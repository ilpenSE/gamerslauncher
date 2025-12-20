#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "logger.h"
#include "loggerstream.hpp"

#include "mgr/appdatamgr.hpp"
#include "mgr/settingsmgr.hpp"

#include "etc/instances.hpp"
#include <QDebug>

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("");
  QCoreApplication::setApplicationName("GamersLauncher");

  // App init
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("GamersLauncher", "Main");

  // Logger init
  qDebug() << AppDataManager::logsPath().constData();
  if (!logger_init(AppDataManager::logsPath().constData(), 1)) {
    qDebug() << "Cannot initialize the logger!";
    return -1;
  }

  // appdata init
  if (!AppDataManager::instance().init()) {
    lerr << "Cannot initialize AppDataManager!";
    return -1;
  }

  // settings mgr init
  if (!_settingsman().init()) {
    lerr << "Cannot initialize SettingsManager!";
    return -1;
  }
  if (!_settingsman().validateJson()) {
    lerr << "Settings validation failed!";
    return -1;
  }

  return app.exec();
}
