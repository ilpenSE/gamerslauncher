#include <QApplication>
#include <iostream>
#include <QMessageBox>

#include "logger.h"
#include "loggerstream.hpp"

#include "mgr/appdata.hpp"
#include "mgr/settings.hpp"

#include "etc/instances.hpp"
#include "etc/enums.hpp"

#include "ui/mainwindow.h"

void showCriticalErrors(Error e, String level) {
  lerr << "FATAL: Cannot initialize " << level << "!";
  lerr << e;
  QMessageBox::critical(nullptr, "Gamers Launcher", String(
                                                        "Cannot initialize %1\nDetails at logs", level));
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("");
  QCoreApplication::setApplicationName("GamersLauncher");

  // Logger init
  if (!logger_init(AppDataManager::logsPath().constData(), 1)) {
    std::cerr << "Cannot initialize the logger!\n";
    QMessageBox::critical(nullptr, "Gamers Launcher", "Cannot initialize logger!");
    return -1;
  }

  // appdata init
  auto appDataInitResult = _appdataman().init();
  if (appDataInitResult.isError()) {
    showCriticalErrors(appDataInitResult.error(), "appdata");
    return -1;
  }

  // settings mgr init
  auto settingsInitResult = _settingsman().init();
  if (settingsInitResult.isError()) {
    showCriticalErrors(settingsInitResult.error(), "settings");
    return -1;
  }

  // validate settings
  auto settingsValidateResult = _settingsman().validateJson();
  if (settingsValidateResult.isError()) {
    showCriticalErrors(settingsValidateResult.error(), "settings");
    return -1;
  }

  // init language mgr
  auto languageInitResult = _langman().init();
  if (languageInitResult.isError()) {
    showCriticalErrors(settingsValidateResult.error(), "language");
    return -1;
  }

  // change lang on first run and flag first run as true
  if (_settingsman().get("FirstRun").value().toBool()) {
    Language systemLang = _langman().getSystemLanguage();
    _settingsman().set("FirstRun", false);
    _langman().changeLanguage(systemLang);
  }

  MainWindow w;
  w.show();
  return app.exec();  
}
