#ifndef APPDATAMANAGER
#define APPDATAMANAGER

#include <QObject>
#include "../etc/result.hpp"

class AppDataManager : public QObject {
  Q_OBJECT
 public:
  static AppDataManager& instance() {
    static AppDataManager _instance;
    return _instance;
  }

  static QString cfgPath(); // configs, general
  static QString mcPath(); // .minecraft
  static QByteArray logsPath(); // cfg/logs
  ErrorOrNot init();

 private:
  explicit AppDataManager(QObject* parent = nullptr) : QObject(parent) {}
  AppDataManager(const AppDataManager&) = delete;
  AppDataManager& operator=(const AppDataManager&) = delete;
  ~AppDataManager() override = default;
};

#endif
