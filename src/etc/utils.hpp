#ifndef UTILS
#define UTILS

#include "result.hpp"

#include <QDir>
#include "string.hpp"

class Utils {
 public:

  /**
   * Checks if given path exists. If not, it tries to create that path.
   * If creation of path fails, function returns false
   * This is commonly used in AppDataManager
   * @brief checkAndCreateDir
   * @param path
   * @return if path create succeeded when path doesnt exist
   */
  static ErrorOrNot checkAndCreateDir(const QString& path) {
    QDir dir(path);
    if (!dir.exists()) {
      if (!dir.mkpath(".")) {
        return Error{ErrorCode::InternalError, String("Cannot create path: ", dir.absolutePath())};
      }
    }
    return std::monostate{};
  }

 private:
};

#endif
