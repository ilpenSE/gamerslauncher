#include "settings.hpp"

#include <QFile>

#include "appdata.hpp"
#include "loggerstream.hpp"
#include "../util/jsonworker.hpp"
#include "../util/jsonvalidator.hpp"
#include <QDir>

ErrorOrNot SettingsManager::init() {
  // initializing variables
  m_filePath = QDir(AppDataManager::cfgPath()).filePath(SETTINGS_JSON_NAME);

  // check if settings.json exists
  if (QFile::exists(m_filePath)) {
    auto res = JsonWorker::loadJson(m_filePath);
    if (res.isError()) return res.error();
    m_settings = res.value();
  } else {
    auto res = saveOrCreateJson();
    if (res.isError()) return res.error();
  }

  linfo << "SettingsManager has been successfully initialized";
  return std::monostate{};
}

ErrorOrNot SettingsManager::validateJson() {
  auto functionResult = JsonValidator::validateSchema(m_settings, settingsSchema, true);
  if (functionResult.isError()) return functionResult.error();

  ValidationResult validationResult = functionResult.value();

  if (!validationResult.isValid) {
    return Error{ErrorCode::ValidationFailed, validationResult.errorMessage};
  }

  // only change JSON when validator changed her
  if (validationResult.jsonChanged) {
    m_settings = validationResult.fixedValue.toObject();
    JsonWorker::saveJson(m_filePath, m_settings);
  }

  return std::monostate{};
}

ErrorOrNot SettingsManager::set(ConstString fieldName, QJsonValue newValue) {
  if (!m_settings.contains(fieldName)) return Error{ErrorCode::NotFound,
                                                    String("[SET] No field named %1 in settings", fieldName)};
  if (newValue == m_settings[fieldName]) return std::monostate{};

  m_settings[fieldName] = newValue;
  return std::monostate{};
}

Expected<QJsonValue> SettingsManager::get(ConstString fieldName) {
  if (!m_settings.contains(fieldName)) return Error{ErrorCode::NotFound,
                                                    String("[GET] No field named %1 in settings", fieldName)};
  return QJsonValue{m_settings[fieldName]};
}

ErrorOrNot SettingsManager::saveOrCreateJson() {
  return JsonWorker::saveJson(m_filePath, m_settings);
}
