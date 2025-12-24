#ifndef INSTANCES
#define INSTANCES

#include "../mgr/appdata.hpp"
#include "../mgr/settings.hpp"
#include "../mgr/language.hpp"

#define _langman() LanguageManager::instance()
#define _appdataman() AppDataManager::instance()
#define _settingsman() SettingsManager::instance()

#endif
