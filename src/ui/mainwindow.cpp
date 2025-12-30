#include "mainwindow.h"

#include "ui_mainwindow.h"

#include "loggerstream.hpp"
#include <etc/instances.hpp>
#include <util/jsonworker.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

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
