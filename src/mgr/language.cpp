#include "language.hpp"
#include <QLocale>
#include <etc/instances.hpp>
#include <etc/enums.hpp>

#include <QApplication>
#include "loggerstream.hpp"

ErrorOrNot LanguageManager::init() {

  return std::monostate{};
}

bool LanguageManager::changeLanguage(Language lang) {
  QString langCode = Language_s::toString(lang);
  if (langCode == m_currentLanguage) return true;

  // remove old translator
  if (m_translator) {
    qApp->removeTranslator(m_translator);
    delete m_translator;
  }

  // load new translator
  m_translator = new QTranslator(this);
  if (!m_translator->load(":/assets/translations/" + langCode + ".qm")) {
    lerr << String("Cannot load %1 language", langCode);
    return false;
  }

  qApp->installTranslator(m_translator);
  m_currentLanguage = langCode;

  emit languageChanged();

  auto setRes = _settingsman().set("Language", langCode);
  if (!setRes.isError()) {
    lerr << String("Cannot set language %1 into settings", langCode);
    lerr << setRes.error();
    return false;
  }

  return true;
}

bool LanguageManager::changeLanguage(ConstQString lang) {
  return changeLanguage(Language_s::fromString(lang));
}

Language LanguageManager::getSystemLanguage() {
  QLocale locale = QLocale::system();
  QString langCode = locale.name().split('_').first();
  return Language_s::fromString(langCode);
}
