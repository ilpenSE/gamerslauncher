#include "appdatamgr.hpp"

#include <QStandardPaths>
#include <QDir>

#include "loggerstream.hpp"
#include "../etc/utils.hpp"

QString AppDataManager::cfgPath() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QByteArray AppDataManager::logsPath() {
  return QDir(cfgPath()).filePath("logs").toUtf8();
}

QString AppDataManager::mcPath() {
  return QDir(cfgPath()).filePath("minecraft");
}

bool AppDataManager::init() {
  // config dir (settings.json etc.)
  if (!Utils::checkAndCreateDir(cfgPath())) return false;

  // logs folder is handled by logger lib

  // minecraft dir
  if (!Utils::checkAndCreateDir(mcPath())) return false;

  linfo << "AppDataManager has been successfully initialized";
  return true;
}
