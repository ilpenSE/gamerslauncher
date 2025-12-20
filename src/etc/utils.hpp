#ifndef UTILS
#define UTILS

#include "enums.hpp"
#include "loggerstream.hpp"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class Utils {
 public:

  /**
   * Checks if given path exists. If not, it tries to create that path.
   * If creation of path fails, function returns false
   * This is commonly used in AppDataManager
   * @brief checkAndCreateDir
   * @param path
   * @return if folder create succeeded
   */
  static bool checkAndCreateDir(const QString& path) {
    QDir dir(path);
    if (!dir.exists()) {
      if (!dir.mkpath(".")) {
        lerr << "Cannot create path: " << dir.absolutePath();
        return false;
      }
    }
    return true;
  }

 private:
};

class JsonWorker {
 public:
  static QJsonDocument loadJsonDoc(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      lerr << "JSON file cannot be opened: " << filepath;
      return {};
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
      lerr << "JSON file parse error: " + parseError.errorString() + ", in file: " + filepath;
      return {};
    }

    return doc;
  }

  static QJsonObject loadJson(const QString& filepath) {
    QJsonDocument d = loadJsonDoc(filepath);
    if (!d.isObject()) {
      lerr << filepath + " is not an JSON object!";
      return {};
    }
    return loadJsonDoc(filepath).object();
  }

  static QJsonArray loadJsonArr(const QString& filepath) {
    QJsonDocument d = loadJsonDoc(filepath);
    if (!d.isArray()) {
      lerr << filepath + " is not an JSON array!";
      return {};
    }
    return loadJsonDoc(filepath).array();
  }

  static bool saveJson(const QString& filepath, const QJsonObject& obj) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      lwarn << "Cannot write json to " << filepath;
      return false;
    }
    QJsonDocument doc(obj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
  }

  static bool saveJson(const QString& filepath, const QJsonArray& arr) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      lwarn << "Cannot write json to " << filepath;
      return false;
    }
    QJsonDocument doc(arr);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
  }
 private:
};

#endif
