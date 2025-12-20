#ifndef ENUMS
#define ENUMS

#include <QString>
#include <QMap>

// =========== ENUMS ===========
enum class Language {
  ENGLISH, TURKISH, GERMAN
};

enum class ClientType {
  VANILLA, FABRIC, FORGE, CUSTOM, OPTIFINE
};

// =========== STRUCTS ===========
struct Language_s {
  inline static const QMap<QString, Language> map = {
      {"en", Language::ENGLISH}, {"tr", Language::TURKISH},
      {"de", Language::GERMAN}
  };

  inline static QString toString(const Language& lang) {
    switch(lang) {
      case Language::ENGLISH: return "en";
      case Language::TURKISH: return "tr";
      case Language::GERMAN: return "de";
      default: return "en";
    }
  }

  inline static Language fromString(const QString& str) {
    return map.value(str, Language::ENGLISH);
  }
};

struct ClientType_s {
  inline static const QMap<QString, ClientType> map = {
      {"vanilla", ClientType::VANILLA}, {"fabric", ClientType::FABRIC},
      {"forge", ClientType::FORGE}, {"optifine", ClientType::OPTIFINE},
      {"custom", ClientType::CUSTOM}
  };

  inline static QString toString(const ClientType& lang) {
    switch(lang) {
      case ClientType::VANILLA: return "vanilla";
      case ClientType::FABRIC: return "fabric";
      case ClientType::FORGE: return "forge";
      case ClientType::CUSTOM: return "custom";
      case ClientType::OPTIFINE: return "optifine";
      default: return "vanilla";
    }
  }

  inline static ClientType fromString(const QString& str) {
    return map.value(str, ClientType::VANILLA);
  }
};

#endif
