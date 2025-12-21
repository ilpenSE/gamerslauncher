#ifndef RESULT_HPP
#define RESULT_HPP

#include <variant>
#include "string.hpp"

enum class ErrorCode {
  InvalidArgument,
  NotFound,
  ValidationFailed,
  InternalError
};

/**
 * @brief The Error struct, for better error handling
 */
struct Error {
  ErrorCode code;
  String message;
};

template <typename E, typename T>
class Either {
 public:
  Either(const E& e) : data(e) {}
  Either(const T& v) : data(v) {}

  bool isError() const { return std::holds_alternative<E>(data); }
  const E& error() const { return std::get<E>(data); }
  const T& value() const { return std::get<T>(data); }

  const String errormsg() const {
    if constexpr (std::is_same_v<E, Error>) {
      return std::get<Error>(data).message;
    } else {
      return "";
    }
  }

 private:
  std::variant<E, T> data;
};

template <typename T>
using Expected = Either<Error, T>;

// use this instead of Expected<bool>, if bool is just a error state
using ErrorOrNot = Expected<std::monostate>;

#endif  // RESULT_HPP
