#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "logger.h"
#include "loggerstream.hpp"

#include "mgr/appdata.hpp"
#include "mgr/settings.hpp"

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
  auto appDataInitResult = _appdataman().init();
  if (appDataInitResult.isError()) {
    lerr << "Cannot initialize AppDataManager!";
    lerr << appDataInitResult.errormsg();
    return -1;
  }

  // settings mgr init
  auto settingsInitResult = _settingsman().init();
  if (settingsInitResult.isError()) {
    lerr << "Cannot initialize SettingsManager!";
    lerr << settingsInitResult.errormsg();
    return -1;
  }

  // validate settings
  auto settingsValidateResult = _settingsman().validateJson();
  if (settingsValidateResult.isError()) {
    lerr << "Settings validation failed!";
    lerr << settingsValidateResult.errormsg();
    return -1;
  }

  // change lang on first run and flag first run as true
  if (_settingsman().get("FirstRun").value().toBool()) {
    _settingsman().set("FirstRun", false);
  }

  return app.exec();
}
