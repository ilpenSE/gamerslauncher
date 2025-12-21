#include "jsonvalidator.hpp"

JsonValidator::JsonValidator() {}

/**
 * Working for JSON objects
 * not tested on array and pure values
 *
 * needs to be optimized and cleared
 */

Expected<ValidationResult> JsonValidator::validateSchema(
    const QJsonValue& value,
    CJsonSchemaRef schema,
    bool canChangeJson) {
  ValidationResult result = {};

  // DO NOT validate if schema is empty
  if (schema.isEmpty()) {
    result.fixedValue = value;
    return result;
  }

  // for objects
  if (value.isObject()) return handleValidateObjects(value.toObject(), schema, canChangeJson);

  // for arrays
  if (value.isArray()) return handleValidateArrays(value.toArray(), schema, canChangeJson);

  // for primitive elements, single element schema
  if (schema.size() == 1) return handleValidatePrimitives(value, schema, canChangeJson);

  result.isValid = false;
  result.errorMessage = "Value type doesn't match schema structure";
  return result;
}

Expected<ValidationResult> JsonValidator::handleValidateObjects(const QJsonObject& obj,
                                                 CJsonSchemaRef schema,
                                                 bool canChangeJson) {
  ValidationResult result = {};
  QJsonObject resObj = obj;

  for (const auto& field : std::as_const(schema)) {
    bool exists = obj.contains(field.name);
    QJsonValue val = exists ? obj[field.name] : QJsonValue();

    // STRICT: HAVE TO exist and be exactly defaultValue
    if (field.constraints.has(JsonConstraints::STRICT_MODE)) {
      if (!exists || val != field.defaultValue) {
        if (canChangeJson) {
          resObj[field.name] = field.defaultValue;
          result.jsonChanged = true;
        } else {
          result.isValid = false;
          result.errorMessage = String("Field '%1' must be exactly '%2'", field.name,
                                        field.defaultValue.toVariant().toString());
          return result;
        }
      }
      continue; // STRICT control enough, no need to type-check
    }

    // NOTNULL: HAVE TO exist, MUST NOT be NULL
    if (field.constraints.has(JsonConstraints::NOTNULL) && (!exists || val.isNull())) {
      if (canChangeJson) {
        resObj[field.name] = field.defaultValue;
        result.jsonChanged = true;
        continue;
      } else {
        result.isValid = false;
        result.errorMessage = String("Field '%1' cannot be null", field.name);
        return result;
      }
    }

    // REQUIRED: HAVE TO exist (NULLABLE)
    if (field.constraints.has(JsonConstraints::REQUIRED) && !exists) {
      if (canChangeJson) {
        resObj[field.name] = field.defaultValue;
        result.jsonChanged = true;
        continue;
      } else {
        result.isValid = false;
        result.errorMessage = String("Required field '%1' is missing", field.name);
        return result;
      }
    }

    // Field yoksa ve REQUIRED değilse skip
    if (!exists) continue;

    // Type validation
    if (field.types != JsonTypes::NONE && !val.isNull()) {
      if (!validateType(val, field.types)) {
        if (canChangeJson) {
          resObj[field.name] = field.defaultValue;
          result.jsonChanged = true;
        } else {
          result.isValid = false;
          result.errorMessage = String("Field '%1' has invalid type", field.name);
          return result;
        }
      }
    }

    // Nested validation (if children exist)
    if (!field.children.isEmpty() && (val.isObject() || val.isArray())) {
      auto nestedResult = validateSchema(val, field.children, canChangeJson);

      if (nestedResult.isError()) {
        return nestedResult;
      }

      auto nested = nestedResult.value();
      if (!nested.isValid) {
        result.isValid = false;
        result.errorMessage = String("Field '%1': %2", field.name, nested.errorMessage);
        return result;
      }

      if (nested.jsonChanged) {
        resObj[field.name] = nested.fixedValue;
        result.jsonChanged = true;
      }
    }
  }

  if (result.jsonChanged) {
    result.fixedValue = resObj;
  }

  return result;
}

Expected<ValidationResult> JsonValidator::handleValidateArrays(const QJsonArray& arr,
                                                CJsonSchemaRef schema,
                                                bool canChangeJson) {
  ValidationResult result = {};
  QJsonArray resArr = arr;

  if (schema.isEmpty()) {
    return result;
  }

  // İlk şema elementi array içindeki her element için geçerli
  const auto& elementSchema = schema[0];
  bool arrayChanged = false;

  for (int i = 0; i < arr.size(); ++i) {
    QJsonValue element = arr[i];

    // STRICT check
    if (elementSchema.constraints.has(JsonConstraints::STRICT_MODE)) {
      if (element != elementSchema.defaultValue) {
        if (canChangeJson) {
          resArr[i] = elementSchema.defaultValue;
          arrayChanged = true;
        } else {
          result.isValid = false;
          result.errorMessage = String("Array[%1] must be exactly '%2'", i,
                                        elementSchema.defaultValue.toVariant().toString());
          return result;
        }
      }
      continue;
    }

    // NOTNULL check
    if (elementSchema.constraints.has(JsonConstraints::NOTNULL) && element.isNull()) {
      if (canChangeJson) {
        resArr[i] = elementSchema.defaultValue;
        arrayChanged = true;
        continue;
      } else {
        result.isValid = false;
        result.errorMessage = String("Array[%1] cannot be null", i);
        return result;
      }
    }

    // Type validation
    if (elementSchema.types != JsonTypes::NONE && !element.isNull()) {
      if (!validateType(element, elementSchema.types)) {
        if (canChangeJson) {
          resArr[i] = elementSchema.defaultValue;
          arrayChanged = true;
        } else {
          result.isValid = false;
          result.errorMessage = String("Array[%1] has invalid type", i);
          return result;
        }
      }
    }

    // Nested validation
    if (!elementSchema.children.isEmpty() && (element.isObject() || element.isArray())) {
      auto nestedResult = validateSchema(element, {elementSchema.children}, canChangeJson);

      if (nestedResult.isError()) {
        return nestedResult;
      }

      auto nested = nestedResult.value();
      if (!nested.isValid) {
        result.isValid = false;
        result.errorMessage = String("Array[%1]: %2", i, nested.errorMessage);
        return result;
      }

      if (nested.jsonChanged) {
        resArr[i] = nested.fixedValue;
        arrayChanged = true;
      }
    }
  }

  if (arrayChanged) {
    result.fixedValue = resArr;
    result.jsonChanged = true;
  }

  return result;
}

Expected<ValidationResult> JsonValidator::handleValidatePrimitives(const QJsonValue& value,
                                                                   CJsonSchemaRef schema,
                                                                   bool canChangeJson) {
  ValidationResult result = {};
  const auto& field = schema[0];

  // STRICT check
  if (field.constraints.has(JsonConstraints::STRICT_MODE)) {
    if (value != field.defaultValue) {
      if (canChangeJson) {
        result.fixedValue = field.defaultValue;
        result.jsonChanged = true;
      } else {
        result.isValid = false;
        result.errorMessage = String("Value must be exactly '%1'", field.defaultValue);
      }
    }
    return result;
  }

  // NOTNULL check
  if (field.constraints.has(JsonConstraints::NOTNULL) && value.isNull()) {
    if (canChangeJson) {
      result.fixedValue = field.defaultValue;
      result.jsonChanged = true;
    } else {
      result.isValid = false;
      result.errorMessage = "Value cannot be null";
    }
    return result;
  }

  // Type validation
  if (field.types != JsonTypes::NONE && !value.isNull()) {
    if (!validateType(value, field.types)) {
      if (canChangeJson) {
        result.fixedValue = field.defaultValue;
        result.jsonChanged = true;
      } else {
        result.isValid = false;
        result.errorMessage = "Invalid type";
      }
    }
  }

  return result;
}

bool JsonValidator::validateType(CJsonValueRef val, JsonType allowedTypes) {
  static const QHash<QJsonValue::Type, JsonType> typeMap = {
      {QJsonValue::Null,   JsonTypes::JNULL},
      {QJsonValue::Bool,   JsonTypes::BOOLEAN},
      {QJsonValue::Double, JsonTypes::NUMBER},
      {QJsonValue::String, JsonTypes::STRING},
      {QJsonValue::Array,  JsonTypes::ARRAY},
      {QJsonValue::Object, JsonTypes::OBJECT}
  };

  auto it = typeMap.find(val.type());
  if (it == typeMap.end()) {
    return false;
  }

  return allowedTypes.has(it.value());
}
