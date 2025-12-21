#ifndef JSONVALIDATOR
#define JSONVALIDATOR

#include "../etc/result.hpp"
#include "jsonstructs.hpp"

#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

class JsonValidator {
 public:
  JsonValidator();

  // validator dispatcher
  static Expected<ValidationResult> validateSchema(const QJsonValue& jsonValue,
                                                   CJsonSchemaRef schema,
                                                   bool canChangeJson);
 private:
  static Expected<ValidationResult> handleValidateObjects(const QJsonObject& jsonObject,
                                                          CJsonSchemaRef schema,
                                                          bool canChangeJson);

  static Expected<ValidationResult> handleValidateArrays(const QJsonArray& jsonArray,
                                                         CJsonSchemaRef schema,
                                                         bool canChangeJson);

  static Expected<ValidationResult> handleValidatePrimitives(const QJsonValue& jsonValue,
                                                             CJsonSchemaRef schema,
                                                             bool canChangeJson);
  static bool validateType(CJsonValueRef val, JsonType allowedTypes);

};

#endif
