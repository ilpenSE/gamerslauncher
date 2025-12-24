#include <QApplication>
#include <iostream>

#include "logger.h"
#include "loggerstream.hpp"

#include "mgr/appdata.hpp"
#include "mgr/settings.hpp"

#include "etc/instances.hpp"
#include "etc/enums.hpp"

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("");
  QCoreApplication::setApplicationName("GamersLauncher");
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  // Logger init
  if (!logger_init(AppDataManager::logsPath().constData(), 1)) {
    std::cerr << "Cannot initialize the logger!\n";
    return -1;
  }

  // appdata init
  auto appDataInitResult = _appdataman().init();
  if (appDataInitResult.isError()) {
    lerr << "Cannot initialize AppDataManager!";
    lerr << appDataInitResult.error();
    return -1;
  }

  // settings mgr init
  auto settingsInitResult = _settingsman().init();
  if (settingsInitResult.isError()) {
    lerr << "Cannot initialize SettingsManager!";
    lerr << settingsInitResult.error();
    return -1;
  }

  // validate settings
  auto settingsValidateResult = _settingsman().validateJson();
  if (settingsValidateResult.isError()) {
    lerr << "Settings validation failed!";
    lerr << settingsValidateResult.error();
    return -1;
  }

  // App init
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  // init language mgr
  auto languageInitResult = _langman().init();
  if (languageInitResult.isError()) {
    lerr << "Cannot initialize LanguageManager!";
    lerr << languageInitResult.error();
    return -1;
  }

  // change lang on first run and flag first run as true
  if (_settingsman().get("FirstRun").value().toBool()) {
    Language systemLang = _langman().getSystemLanguage();
    _settingsman().set("FirstRun", false);
    _langman().changeLanguage(systemLang);
  }

  engine.loadFromModule("GamersLauncher", "Main");
  return app.exec();
}
