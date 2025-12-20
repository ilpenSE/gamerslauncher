#ifndef SETTINGSMANAGER
#define SETTINGSMANAGER

#include <QObject>
#include <QJsonObject>

#include "versionmgr.hpp"
#include "../etc/enums.hpp"

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
  bool init();

  // main validator
  bool validateJson();

 private:
  QJsonObject m_settings;
  QString m_filePath;

  // default settings.json values
  QJsonObject defaultSchema = {
    {"Language", "en"}, {"Version", VERSION_STR}, {"FirstRun", false},
    {"MinecraftVersion", "latest"}, {"ClientType", "vanilla"}
  };

  // if this is true, this should mean settings are corrupted
  bool needsUpdate = false;

  // validator helpers (all returns needsUpdate)
  template <typename Enum>
  bool validateEnum(const QJsonValue& provided) {
    if (needsUpdate) return true;

    if constexpr (std::is_same_v<Enum, Language>) {
      // validateEnum<Language>
      return !Language_s::map.contains(provided.toString());
    } else if constexpr (std::is_same_v<Enum, ClientType>) {
      // func used like validateEnum<ClientType>
      return !ClientType_s::map.contains(provided.toString());
    } else {
      static_assert(sizeof(Enum) == 0, "Unsupported enum type!");
    }
    return false;
  }
  bool runTypeChecks();
  bool validateVersion();

  // quick wrapper
  bool saveOrCreateJson();

  // singleton utils
  explicit SettingsManager(QObject* parent = nullptr) : QObject(parent) {}
  SettingsManager(const SettingsManager&) = delete;
  SettingsManager& operator=(const SettingsManager&) = delete;
  ~SettingsManager() override = default;
};

#endif
