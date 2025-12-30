#ifndef LANGUAGE
#define LANGUAGE

#include <QObject>
#include <QTranslator>

#include <etc/result.hpp>
#include <etc/string.hpp>
#include <etc/enums.hpp>

class LanguageManager : public QObject {
  Q_OBJECT
 public:
  static LanguageManager& instance() {
    static LanguageManager _instance;
    return _instance;
  }

  // change languages
  bool changeLanguage(Language lang, bool force = false);
  bool changeLanguage(ConstQString lang, bool force = false);

  // gets system language from OS
  Language getSystemLanguage();

  // init
  ErrorOrNot init();

  QString currentLanguage() const { return m_currentLanguage; }
 signals:
  void languageChanged();
 private:
  QTranslator* m_translator;
  String m_currentLanguage;

  explicit LanguageManager(QObject* parent = nullptr) : QObject(parent),
                                                        m_translator(nullptr),
                                                        m_currentLanguage("en")  {}
  LanguageManager(const LanguageManager&) = delete;
  LanguageManager& operator=(const LanguageManager&) = delete;
  ~LanguageManager() override = default;
};

#endif
