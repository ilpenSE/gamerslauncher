#include "mainwindow.h"

#include "ui_mainwindow.h"

#include "loggerstream.hpp"
#include <etc/instances.hpp>
#include <util/jsonworker.hpp>

#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connectSlots();

  // ===== DEBUG =====
  // settings
  _downloadman().setMaxConcurrentDownloads(6);

  // signal connections
  connect(&_downloadman(), &DownloadManager::downloadProgress,
          [](const QString &url, qint64 received, qint64 total) {
            linfo << "Progress: " << url << '\n' << received << "/" << total;
          });

  connect(&_downloadman(), &DownloadManager::overallProgress,
          [](int completed, int total, qint64 bytes, qint64 totalBytes) {
            if (totalBytes == 0) return;
            linfo << "Overall: " << completed << "/" << total
                     << "(" << (bytes * 100 / totalBytes) << "%)";
          });

  connect(&_downloadman(), &DownloadManager::fileDownloaded,
          [](const QString &url, const QString &dest) {
            linfo << "Downloaded:" << dest;
          });

  connect(&_downloadman(), &DownloadManager::allDownloadsFinished, []() {
    linfo << "All downloads completed!";
  });

  // register download
  _downloadman().registerDownload(DownloadTask{
      "https://github.com/ilpenSE/autoclicker/archive/refs/heads/main.zip",
      "autoclicker.zip",
      "" // make it empty to skip hash checks
  });

  _downloadman().start();
}

void MainWindow::connectSlots() {
  connect(&LanguageManager::instance(),
          &LanguageManager::languageChanged,
          this,
          &MainWindow::retranslate);
}

void MainWindow::retranslate() {
  ui->retranslateUi(this);
}

MainWindow::~MainWindow() {
  auto settingsSaveRes = _settingsman().saveOrCreateJson();
  if (settingsSaveRes.isError()) {
    lerr << "Cannot save settings.json!";
    lerr << settingsSaveRes.error();
  }
  delete ui;
}
