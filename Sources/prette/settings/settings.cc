#include "prette/settings/settings.h"
#include "prette/db_util.h"

namespace prt::settings {
DEFINE_string(settings_dir, "", "The absolute path to store settings. Empty is default.");

  static rx::subject<SettingsEvent*> events_;
  static leveldb::DB* settings_ = nullptr;

  void Settings::Publish(SettingsEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  static inline bool
  HasSettingsDir() {
    return !FLAGS_settings_dir.empty();
  }

  static inline std::string
  GetSettingsDatabaseFilename() {
    if(HasSettingsDir())
      return FLAGS_settings_dir;
    return FLAGS_resources + "/settings";
  }

  rx::observable<SettingsEvent*> Settings::OnEvent() {
    return events_.get_observable();
  }

  void Settings::Initialize() {
    DLOG(INFO) << "initializing settings in " << GetSettingsDatabaseFilename() << "....";
    leveldb::Options options;
    options.create_if_missing = true;
    db::TryOpenWith<>(GetSettingsDatabaseFilename(), options, &settings_);
  }

  bool Settings::VisitSettings(SettingVisitor* vis) {
    leveldb::ReadOptions options;
    leveldb::Iterator* iter = settings_->NewIterator(options);
    for(iter->SeekToFirst(); iter->Valid(); iter->Next()) {
      const auto& key = iter->key();
      const auto& value = iter->value();
    }
    delete iter;

    return false;
  }
}