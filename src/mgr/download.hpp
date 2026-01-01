#ifndef DOWNLOADMANAGER
#define DOWNLOADMANAGER

#include <QObject>
#include <QMap>
#include <etc/result.hpp>
#include <QtNetwork/QNetworkAccessManager>
#include <QQueue>
#include <QFile>
#include <QTimer>
#include <QtNetwork/QNetworkReply>

struct DownloadTask {
  QString targetUrl;
  QString destination;
  QString hash; // if empty, skips hash check

  bool operator==(const DownloadTask& other) const {
    return destination == other.destination &&
           targetUrl == other.targetUrl &&
           hash == other.hash;
  }

  bool operator!=(const DownloadTask& other) const {
    return !(*this == other);
  }
};

/**
 * This manager manages multiple downloads at the same time
 * Uses multi-thread (async) solution
 */
class DownloadManager : public QObject {
  Q_OBJECT
public:
  static DownloadManager& instance() {
    static DownloadManager _instance;
    return _instance;
  }

  // puts task into tasks
  void registerDownload(DownloadTask task);
  void registerDownloads(const QList<DownloadTask>& tasks);

  // these funcs controls queue
  void start();
  void pause();
  void resume();
  void cancel(); // fully cancel

  // settings
  void setMaxConcurrentDownloads(int max) { m_maxConcurrent = max; }
  void setTimeout(int ms) { m_timeoutMs = ms; }
  void setRetryCount(int count) { m_maxRetries = count; }

signals:
  // Progress signals
  void downloadProgress(const QString &url, qint64 received, qint64 total);
  void fileDownloaded(const QString &url, const QString &destination);
  void downloadFailed(const QString &url, const QString &error);

  // General progress
  void overallProgress(int completed, int total, qint64 bytesReceived, qint64 bytesTotal);
  void allDownloadsFinished();

private slots:
  void onDownloadProgress(qint64 received, qint64 total);
  void onDownloadFinished();
  void onDownloadError(QNetworkReply::NetworkError code);
  void onTimeout();

private:
  void startNextDownload(bool force = false);
  bool verifyHash(ConstQString filePath, ConstQString expectedHash);
  void retryDownload(const DownloadTask &task);
  void markAsCompleted(const DownloadTask& task);
  void retry(const DownloadTask& task, ConstString errorMessage = "An error occured");

  QNetworkAccessManager *networkManager;
  QQueue<DownloadTask> downloadQueue;
  QMap<QNetworkReply*, DownloadTask> activeDownloads;
  QMap<QNetworkReply*, QFile*> openFiles;
  QMap<QNetworkReply*, QTimer*> timeouts;
  QMap<QNetworkReply*, qint64> expectedBytes;
  QMap<QNetworkReply*, qint64> receivedBytes;
  QMap<QString, int> retries;  // URL -> retry

  // settings
  int m_maxConcurrent;
  int m_maxRetries;
  int m_timeoutMs;
  int m_completedDownloads;
  int m_totalDownloads;
  qint64 m_totalBytesReceived;
  qint64 m_totalBytesExpected;
  bool m_paused;

  explicit DownloadManager(QObject* parent = nullptr);
  DownloadManager(const DownloadManager&) = delete;
  DownloadManager& operator=(const DownloadManager&) = delete;
  ~DownloadManager();
};

#endif
