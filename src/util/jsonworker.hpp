#ifndef JSONWORKER
#define JSONWORKER

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QJsonValue>

#include "../etc/result.hpp"
#include "../etc/string.hpp"

/**
 * Object/Array:
 *
 * QJsonValue defaultSchema, JsonSchema schema
 */
class JsonWorker {
 public:
  // Json loaders
  static Expected<QJsonDocument> loadJsonDoc(ConstString filepath);
  static Expected<QJsonObject> loadJson(ConstString filepath);
  static Expected<QJsonArray> loadJsonArr(ConstString filepath);

  // Json savers
  static ErrorOrNot saveJson(ConstString filepath, const QJsonObject& obj);
  static ErrorOrNot saveJson(ConstString filepath, const QJsonArray& arr);
};

#endif
