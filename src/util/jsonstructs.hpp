#ifndef JSONSTRUCTS
#define JSONSTRUCTS

#include <QVector>
#include <etc/string.hpp>

// forward decleration
struct JsonType;
struct JsonConstraint;

struct JsonType {
  uint32_t value;

  constexpr JsonType() : value(0) {}
  constexpr JsonType(uint32_t v) : value(v) {}

  constexpr bool has(JsonType other) const {
    return (value & other.value) != 0;
  }

  constexpr JsonType operator|(JsonType other) const {
    return JsonType(value | other.value);
  }

  constexpr JsonType operator&(JsonType other) const {
    return JsonType(value & other.value);
  }

  JsonType& operator|=(JsonType other) {
    value |= other.value;
    return *this;
  }

  constexpr bool operator==(JsonType other) const { return value == other.value; }
  constexpr bool operator!=(JsonType other) const { return value != other.value; }
};

struct JsonConstraint {
  uint32_t value;

  constexpr JsonConstraint() : value(0) {}
  constexpr JsonConstraint(uint32_t v) : value(v) {}

  constexpr bool has(JsonConstraint other) const {
    return (value & other.value) != 0;
  }

  constexpr JsonConstraint operator|(JsonConstraint other) const {
    return JsonConstraint(value | other.value);
  }

  constexpr JsonConstraint operator&(JsonConstraint other) const {
    return JsonConstraint(value & other.value);
  }

  JsonConstraint& operator|=(JsonConstraint other) {
    value |= other.value;
    return *this;
  }

  constexpr bool operator==(JsonConstraint other) const { return value == other.value; }
  constexpr bool operator!=(JsonConstraint other) const { return value != other.value; }
};
// enums / constants
namespace JsonConstraints {
inline constexpr JsonConstraint NONE        {0};
inline constexpr JsonConstraint REQUIRED    {1 << 0};
inline constexpr JsonConstraint NOTNULL     {1 << 1};
inline constexpr JsonConstraint STRICT_MODE {1 << 2};
}

namespace JsonTypes {
inline constexpr JsonType NONE    {0};
inline constexpr JsonType STRING  {1 << 0};
inline constexpr JsonType NUMBER  {1 << 1};
inline constexpr JsonType JNULL   {1 << 2};
inline constexpr JsonType BOOLEAN {1 << 3};
inline constexpr JsonType OBJECT  {1 << 4};
inline constexpr JsonType ARRAY   {1 << 5};
}

// forward decleration
struct JsonField;

/**
 * smth like:
 * {"fieldName", "defaultValue", JsonType, JsonConstraint}
 */
struct JsonField {
  QString name;
  QJsonValue defaultValue;
  JsonType types;
  JsonConstraint constraints;
  QVector<JsonField> children = {};
};

struct ValidationResult {
  bool isValid = true;
  bool jsonChanged = false;  // if worker has changed JSON
  QJsonValue fixedValue;  // Fixed JSON (if jsonChanged=true)
  String errorMessage;  // error message if isValid=false
};

using JsonSchema = QVector<JsonField>;
using CJsonSchemaRef = const QVector<JsonField>&;
using CJsonValueRef = const QJsonValue&;

#endif
