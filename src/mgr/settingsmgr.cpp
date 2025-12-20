#include "settingsmgr.hpp"

#include <QFile>

#include "appdatamgr.hpp"
#include "loggerstream.hpp"
#include "../etc/utils.hpp"
#include "../etc/enums.hpp"

bool SettingsManager::init() {
  // initializing variables
  m_filePath = QDir(AppDataManager::cfgPath()).filePath(SETTINGS_JSON_NAME);

  // check if settings.json exists
  if (!QFile::exists(m_filePath)) {
    if (!saveOrCreateJson()) return false;
    m_settings = defaultSchema;
  } else {
    m_settings = JsonWorker::loadJson(m_filePath);
    if (m_settings.isEmpty()) return false;
  }

  linfo << "SettingsManager has been successfully initialized";
  linfo << "Settings file: " << m_filePath;
  return true;
}

bool SettingsManager::validateJson() {
  needsUpdate = runTypeChecks()
                | validateVersion()
                | validateEnum<Language>(m_settings["Language"])
                | validateEnum<ClientType>(m_settings["ClientType"]);

  if (needsUpdate) return saveOrCreateJson();
  return true;
}

bool SettingsManager::runTypeChecks() {
  if (needsUpdate) return true;

  // range-based loop on string types
  QStringList strings = {"Version", "Language", "ClientType", "MinecraftVersion"};
  for (const QString& field : std::as_const(strings)) {
    if (!m_settings[field].isString()) {
      m_settings[field] = defaultSchema[field]; // set its default value
      return true;
    }
  }

  // since only boolean is FirstRun, we dont need list like in string
  if (!m_settings["FirstRun"].isBool()) {
    m_settings["FirstRun"] = defaultSchema["FirstRun"];
    return true;
  }

  return false;
}

// strictly deterministic value, this needs to be current version str every time
// in future, we can have a list of strict values and a general function to check them
bool SettingsManager::validateVersion() {
  if (needsUpdate) return true;

  QString provided = m_settings["Version"].toString(VERSION_STR);

  if (provided != VERSION_STR) {
    m_settings["Version"] = VERSION_STR;
    return true;
  }

  return false;
}

bool SettingsManager::saveOrCreateJson() {
  return JsonWorker::saveJson(m_filePath, defaultSchema);
}
