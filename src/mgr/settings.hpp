#ifndef SETTINGSMANAGER
#define SETTINGSMANAGER

#include <QObject>
#include <QJsonObject>
#include <QJsonValue>

#include "version.hpp"
#include <etc/result.hpp>
#include "../util/jsonstructs.hpp"

#ifndef SETTINGS_JSON_NAME
#define SETTINGS_JSON_NAME "settings.json"
#endif

class SettingsManager : public QObject {
  Q_OBJECT
 public:
  static SettingsManager& instance() {
    static SettingsManager _instance;
    return _instance;
  }

  // main entry
  ErrorOrNot init();

  // main validator - depr
  ErrorOrNot validateJson();

  // get/set a setting
  ErrorOrNot set(ConstString fieldName, QJsonValue newValue);
  // if returned is empty, settings doesnt contain fieldName
  Expected<QJsonValue> get(ConstString fieldName);

  // get defaults
  String getDefaultLanguage() { return settingsSchema[0].defaultValue.toString(); };
 private:
  QJsonObject m_settings = {
      {"Language", "en"}, {"Version", VERSION_STR}, {"MinecraftVersion", "latest"},
      {"ClientType", "vanilla"}, {"FirstRun", true}
  };
  QString m_filePath;

  // settings json schema for validator
  JsonSchema settingsSchema = {
    // {"FieldName", "DefaultValue", JsonType, JsonConstraint}
    {"Language", "en", JsonTypes::STRING, JsonConstraints::NOTNULL},
    {"Version", VERSION_STR, JsonTypes::STRING, JsonConstraints::STRICT_MODE},
    {"MinecraftVersion", "latest", JsonTypes::STRING, JsonConstraints::NOTNULL},
    {"ClientType", "vanilla", JsonTypes::STRING, JsonConstraints::NOTNULL},
    {"FirstRun", true, JsonTypes::BOOLEAN, JsonConstraints::NOTNULL}
  };

  // if this is true, this should mean settings are corrupted
  bool needsUpdate = false;

  // quick wrapper
  ErrorOrNot saveOrCreateJson();

  // singleton utils
  explicit SettingsManager(QObject* parent = nullptr) : QObject(parent) {}
  SettingsManager(const SettingsManager&) = delete;
  SettingsManager& operator=(const SettingsManager&) = delete;
  ~SettingsManager() override = default;
};

#endif
