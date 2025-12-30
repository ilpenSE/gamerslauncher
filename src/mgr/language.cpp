#include "language.hpp"
#include <QLocale>
#include <etc/instances.hpp>
#include <etc/enums.hpp>

#include <QApplication>
#include "loggerstream.hpp"

ErrorOrNot LanguageManager::init() {
  auto res = _settingsman().get("Language");
  if (res.isError()) return res.error();
  m_currentLanguage = res.value().toString();

  linfo << "Initialized LanguageManager with " << m_currentLanguage;
  return std::monostate{};
}

bool LanguageManager::changeLanguage(Language lang, bool force) {
  QString langCode = Language_s::toString(lang);
  if (langCode == m_currentLanguage && !force) return true;

  // remove old translator
  if (m_translator) {
    qApp->removeTranslator(m_translator);
    m_translator->deleteLater();
  }

  // load new translator
  m_translator = new QTranslator(qApp);
  if (!m_translator->load(":/assets/translations/" + langCode + ".qm")) {
    lerr << String("Cannot load %1 language", langCode);
    return false;
  }

  qApp->installTranslator(m_translator);
  m_currentLanguage = langCode;

  emit languageChanged();

  auto setRes = _settingsman().set("Language", langCode);
  if (setRes.isError()) {
    lerr << String("Cannot set language %1 into settings", langCode);
    lerr << setRes.error();
    return false;
  }

  linfo << "Language changed to " << langCode;
  return true;
}

bool LanguageManager::changeLanguage(ConstQString lang, bool force) {
  return changeLanguage(Language_s::fromString(lang), force);
}

Language LanguageManager::getSystemLanguage() {
  const QLocale locale = QLocale::system();
  const auto uiLangs = locale.uiLanguages();

  QLocale::Language lang = uiLangs.isEmpty()
                               ? locale.language()
                               : QLocale(uiLangs.first()).language();

  switch (lang) {
    case QLocale::Turkish: return Language::TURKISH;
    case QLocale::German:  return Language::GERMAN;
    default:               return Language::ENGLISH;
  }
}
