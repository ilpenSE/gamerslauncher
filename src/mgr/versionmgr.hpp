#ifndef VERSIONMGR
#define VERSIONMGR

#include <QObject>

#ifndef VERSION_STR
#define VERSION_STR "1.0.0"
#endif

// this class will be used in checking updates
class VersionManager : public QObject {
  Q_OBJECT
 public:
  static VersionManager& instance() {
    static VersionManager _instance;
    return _instance;
  }

 private:
  explicit VersionManager(QObject* parent = nullptr) : QObject(parent) {}

  VersionManager(const VersionManager&) = delete;
  VersionManager& operator=(const VersionManager&) = delete;

  ~VersionManager() override = default;
};

#endif
