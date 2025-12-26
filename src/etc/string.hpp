#ifndef STRING_HPP
#define STRING_HPP

#include <QString>
#include <string>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
/**
 * @brief This class overrides, QString's shitty arg formatting
 * and makes it better, it brings c-like formats
 */

class String {
 public:
  String() = default;

  String(const QString& s) : str(s) {}
  String(const char* s) : str(QString::fromUtf8(s)) {}

  template <typename... Args>
  String(const char* fmt, Args&&... args) {
    str = QString::fromUtf8(fmt);
    applyArgs(std::forward<Args>(args)...);
  }

  operator QString() const { return str; }

  std::string toStdString() const {
    return str.toStdString();
  }

  QString toQString() const {
    return str;
  }
 private:
  QString str;

  void applyArgs() {}

  template <typename T, typename... Rest>
  void applyArgs(T&& value, Rest&&... rest) {
    str = str.arg(toQString(std::forward<T>(value)));
    applyArgs(std::forward<Rest>(rest)...);
  }

  static QString toQString(const QString& s) { return s; }
  static QString toQString(const char* s) { return QString::fromUtf8(s); }
  static QString toQString(int v) { return QString::number(v); }
  static QString toQString(double v) { return QString::number(v); }
  static QString toQString(bool v) { return v ? "true" : "false"; }

  static QString toQString(const QJsonValue& v) {
    switch (v.type()) {
      case QJsonValue::Null:
      case QJsonValue::Undefined:
        return "null";

      case QJsonValue::Bool:
        return v.toBool() ? "true" : "false";

      case QJsonValue::Double:
        return QString::number(v.toDouble());

      case QJsonValue::String:
        return v.toString();

      case QJsonValue::Array:
        return QString::fromUtf8(
            QJsonDocument(v.toArray()).toJson(QJsonDocument::Compact)
            );

      case QJsonValue::Object:
        return QString::fromUtf8(
            QJsonDocument(v.toObject()).toJson(QJsonDocument::Compact)
            );
    }
    return {};
  }
};

using ConstString = const String&;
using ConstQString = const QString&;

#endif
