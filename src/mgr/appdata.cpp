#include "appdata.hpp"

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

ErrorOrNot AppDataManager::init() {
  // config dir (settings.json etc.)
  auto configResult = Utils::checkAndCreateDir(cfgPath());
  if (configResult.isError()) return configResult.error();

  // logs folder is handled by logger lib

  // minecraft dir
  auto minecraftResult = Utils::checkAndCreateDir(mcPath());
  if (minecraftResult.isError()) return minecraftResult.error();

  linfo << "AppDataManager has been successfully initialized";
  return std::monostate{};
}
