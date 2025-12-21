#include "jsonworker.hpp"

Expected<QJsonDocument> JsonWorker::loadJsonDoc(ConstString filepath) {
  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return Error{ErrorCode::InternalError, "JSON file cannot be opened: " + filepath};
  }

  QByteArray data = file.readAll();
  file.close();

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
  if (parseError.error != QJsonParseError::NoError) {
    return Error{ErrorCode::InternalError, "JSON file parse error: " + parseError.errorString()
                                                 + ", in file: " + filepath};
  }

  return doc;
}

Expected<QJsonObject> JsonWorker::loadJson(ConstString filepath) {
  auto res = loadJsonDoc(filepath);
  if (res.isError()) return res.error();

  QJsonDocument d = res.value();
  if (!d.isObject()) {
    return Error{ErrorCode::InvalidArgument, filepath + " is not an JSON object!"};
  }
  return d.object();
}

Expected<QJsonArray> JsonWorker::loadJsonArr(ConstString filepath) {
  auto res = loadJsonDoc(filepath);
  if (res.isError()) return res.error();

  QJsonDocument d = res.value();
  if (!d.isArray()) {
    return Error{ErrorCode::InvalidArgument, filepath + " is not an JSON array!"};
  }
  return d.array();
}

ErrorOrNot JsonWorker::saveJson(ConstString filepath, const QJsonObject& obj) {
  QFile file(filepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return Error{ErrorCode::InternalError, "Cannot write json to " + filepath };
  }
  QJsonDocument doc(obj);
  file.write(doc.toJson(QJsonDocument::Indented));
  file.close();
  return std::monostate{};
}

ErrorOrNot JsonWorker::saveJson(ConstString filepath, const QJsonArray& arr) {
  QFile file(filepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return Error{ErrorCode::InternalError, "Cannot write json to " + filepath};
  }
  QJsonDocument doc(arr);
  file.write(doc.toJson(QJsonDocument::Indented));
  file.close();
  return std::monostate{};
}
