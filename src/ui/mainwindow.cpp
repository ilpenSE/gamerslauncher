#include "mainwindow.h"

#include "ui_mainwindow.h"

#include "loggerstream.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  linfo << "Hello From MainWindow";
}

MainWindow::~MainWindow() { delete ui; }
