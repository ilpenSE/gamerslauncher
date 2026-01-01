#include "download.hpp"
#include "loggerstream.hpp"

#include <QDir>
#include <QFileInfo>
#include <etc/utils.hpp>
#include <etc/instances.hpp>

DownloadManager::DownloadManager(QObject* parent) : QObject(parent)
  , networkManager(new QNetworkAccessManager(this))
  , m_maxConcurrent(4)
  , m_maxRetries(3)
  , m_timeoutMs(30000)  // 30 secs
  , m_completedDownloads(0)
  , m_totalDownloads(0)
  , m_totalBytesReceived(0)
  , m_totalBytesExpected(0)
  , m_paused(false) {}

DownloadManager::~DownloadManager() {
  // Cleanup
  for (auto reply : activeDownloads.keys()) {
    reply->abort();
    reply->deleteLater();
  }

  for (auto file : openFiles.values()) {
    file->close();
    delete file;
  }
}

void DownloadManager::registerDownload(DownloadTask task) {
  ConstQString mcPath = _appdataman().mcPath();
  if (!task.destination.startsWith(mcPath))
    task.destination = QDir(mcPath).filePath(task.destination);

  if (downloadQueue.contains(task)) return;
  downloadQueue.enqueue(task);
  m_totalDownloads++;
}

void DownloadManager::registerDownloads(const QList<DownloadTask>& tasks) {
  for (const auto& t : std::as_const(tasks))
    registerDownload(t);
}

// QUEUE FUNCS
void DownloadManager::start() {
  m_paused = false;
  m_completedDownloads = 0;
  m_totalBytesReceived = 0;

  // Maksimum concurrent download kadar başlat
  for (int i = 0; i < m_maxConcurrent && !downloadQueue.isEmpty(); ++i) {
    startNextDownload();
  }
}

void DownloadManager::pause() {
  m_paused = true;
  // abort reply but dont delete
  for (auto reply : activeDownloads.keys()) {
    reply->abort();
  }
}

void DownloadManager::resume() {
  m_paused = false;

  // resume
  while (activeDownloads.size() < m_maxConcurrent && !downloadQueue.isEmpty()) {
    startNextDownload();
  }
}

void DownloadManager::cancel() {
  m_paused = true;
  downloadQueue.clear();

  // fully abort and delete
  for (auto reply : activeDownloads.keys()) {
    reply->abort();
    reply->deleteLater();
  }

  activeDownloads.clear();
}

// a little helper to mark a download as completed and update bytes
void DownloadManager::markAsCompleted(const DownloadTask& task) {
  m_completedDownloads++;

  emit fileDownloaded(task.targetUrl, task.destination);
  emit overallProgress(m_completedDownloads, m_totalDownloads,
                       m_totalBytesReceived, m_totalBytesExpected);
}

// helper that handles retry logic
void DownloadManager::retry(const DownloadTask& task, ConstString errorMessage) {
  if (retries[task.targetUrl] < m_maxRetries) {
    retryDownload(task);
  } else {
    emit downloadFailed(task.targetUrl, errorMessage);
    QFile::remove(task.destination);
  }
}

void DownloadManager::startNextDownload(bool force)
{
  // firstly, check that all downloads finished or not
  if (downloadQueue.isEmpty() && activeDownloads.isEmpty()) {
    if (m_completedDownloads == m_totalDownloads && m_totalDownloads > 0) {
      linfo << "All downloads finished!";
      emit allDownloadsFinished();
    }
    return;
  }

  if (m_paused || downloadQueue.isEmpty()) return;

  // get first element and deq it
  DownloadTask task = downloadQueue.dequeue();

  // check and create dest path
  QFileInfo fi(task.destination);
  auto pathRes = Utils::checkAndCreateDir(fi.absolutePath());
  if (pathRes.isError()) {
    lerr << pathRes.error();
    lerr << "[FS] Cannot create path: " << task.destination;
    return;
  }

  // ensured that dest path exists, ensure that file exists
  // if force = true, FORCE THE DOWNLOAD, SKIP HASH CHECKS
  if (!force && QFile::exists(task.destination) && !task.hash.isEmpty()) {
    if (!task.hash.isEmpty() && verifyHash(task.destination, task.hash)) {
      linfo << "[FS] File already exists and verified: " << task.destination;
      linfo << "Marking as completed and skipping to next...";
      markAsCompleted(task);
      startNextDownload();
      return;
    }
  }

  // start network request to download
  QNetworkRequest request(task.targetUrl);
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::NoLessSafeRedirectPolicy);
  QNetworkReply *reply = networkManager->get(request);

  // create file handler
  QFile *file = new QFile(task.destination);
  if (!file->open(QIODevice::WriteOnly)) {
    // ABORT REQUEST, DELETE FILE AND REPLY, SKIP TO NEXT
    lerr << "[FS] Cannot open file for writing: " << task.destination;
    lerr << "Aborting and deleting the request and skipping to the next...";
    emit downloadFailed(task.targetUrl, "[FS] Cannot open file for writing");
    delete file;
    reply->deleteLater();
    startNextDownload();
    return;
  }

  // update maps
  activeDownloads[reply] = task;
  openFiles[reply] = file;

  // timeout timer
  QTimer *timer = new QTimer(this);
  timer->setSingleShot(true);
  timer->setInterval(m_timeoutMs);
  connect(timer, &QTimer::timeout, this, &DownloadManager::onTimeout);
  timeouts[reply] = timer;
  timer->start();

  // signal connections
  connect(reply, &QNetworkReply::downloadProgress,
          this, &DownloadManager::onDownloadProgress);
  connect(reply, &QNetworkReply::finished,
          this, &DownloadManager::onDownloadFinished);
  connect(reply, &QNetworkReply::errorOccurred,
          this, &DownloadManager::onDownloadError);

  // Veriyi dosyaya yaz
  connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
    file->write(reply->readAll());
  });

  linfo << "Started to download: " << task.targetUrl;
}

void DownloadManager::onDownloadProgress(qint64 received, qint64 total)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply || !activeDownloads.contains(reply)) return;

  if (total > 0 && !expectedBytes.contains(reply)) {
    expectedBytes[reply] = total;
    m_totalBytesExpected += total;
  }

  // reset timeout
  if (timeouts.contains(reply)) {
    timeouts[reply]->start();
  }

  qint64 prev = receivedBytes.value(reply, 0);
  receivedBytes[reply] = received;
  m_totalBytesReceived += (received - prev);

  const DownloadTask &task = activeDownloads[reply];
  emit downloadProgress(task.targetUrl, received, total);
  emit overallProgress(
      m_completedDownloads,
      m_totalDownloads,
      m_totalBytesReceived,
      m_totalBytesExpected
  );
}

void DownloadManager::onDownloadFinished()
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply || !activeDownloads.contains(reply)) return;

  DownloadTask task = activeDownloads.take(reply);
  QFile *file = openFiles.take(reply);
  QTimer *timer = timeouts.take(reply);

  timer->stop();
  delete timer;

  // write buffer and close file
  file->write(reply->readAll());
  file->close();
  delete file;

  // handle network errors
  if (reply->error() != QNetworkReply::NoError) {
    lerr << "[NETWORK] Failed to download " << task.targetUrl << " in " << task.destination;
    // retry logic
    retry(task, reply->errorString());
    reply->deleteLater();
    startNextDownload();
    return;
  }

  // hash verification
  if (!task.hash.isEmpty() && !verifyHash(task.destination, task.hash)) {
    lwarn << "Hash mismatch for: " << task.targetUrl << ", Retrying...";
    // retry logic
    retry(task, "Hash verification failed");
    reply->deleteLater();
    startNextDownload();
    return;
  }

  // success
  linfo << "Downloaded successfully: " << task.destination;
  markAsCompleted(task);
  m_totalBytesReceived += expectedBytes.value(reply, 0);
  expectedBytes.remove(reply);
  receivedBytes.remove(reply);

  // cleanup
  retries.remove(task.targetUrl);
  reply->deleteLater();

  // go next
  startNextDownload();
}

void DownloadManager::onDownloadError(QNetworkReply::NetworkError code)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
  if (!reply || !activeDownloads.contains(reply)) return;
  if (reply->error() == QNetworkReply::NoError) return; // no actual error
  lerr << reply->errorString();
  return;
}

void DownloadManager::onTimeout()
{
  QTimer *timer = qobject_cast<QTimer*>(sender());

  // find reply via timer
  QNetworkReply *reply = timeouts.key(timer);
  if (!reply) {
    return;
  }

  lwarn << "Download timeout:" << activeDownloads[reply].targetUrl;
  reply->abort(); // this triggers onDownloadFinished
}

bool DownloadManager::verifyHash(const QString &filePath, const QString &expectedHash)
{
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QCryptographicHash hash(QCryptographicHash::Sha1);
  if (!hash.addData(&file)) {
    return false;
  }

  QString actualHash = hash.result().toHex();
  return actualHash.compare(expectedHash, Qt::CaseInsensitive) == 0;
}

void DownloadManager::retryDownload(const DownloadTask &task)
{
  retries[task.targetUrl]++;
  linfo << "Retrying download: " << task.targetUrl
           << "(" << retries[task.targetUrl] << "/" << m_maxRetries << ")";

  // add back to queue (so startNextDownload calls)
  downloadQueue.enqueue(task);
}
