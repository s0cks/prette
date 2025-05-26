#include "prette/settings/settings.h"

#include <chrono>
#include <filesystem>
#include <fmt/format.h>
#include <glog/logging.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/event.h"
#include "prette/settings/settings_event.h"
#include "prette/thread_local.h"

namespace prt {
DEFINE_GLOBAL_EVENT_SUBJECT(SettingsEvent, events);
static ThreadLocal<Settings::DB> db_{};

void Settings::PublishEvent(SettingsEvent* event) {
  ASSERT(event);
  const auto& subscriber = events.get_subscriber();
  return subscriber.on_next(event);
}

Settings::Settings(const fs::path root) :
  path_(root) {
  Load();
}

Settings::~Settings() {
  Save();
  delete db_;
}

void Settings::Save() {
  PRT_PROFILING_BEGIN(saving);
  {
    PublishSavingSettingsEvent();
    // TODO: generate write batch for batching saves
    PublishSettingsSavedEvent();
  }
  PRT_PROFILING_END(saving);
  DVLOG(1) << "settings saved in " << std::chrono::duration_cast<std::chrono::milliseconds>(saving_duration).count()
           << "ms.";
}

void Settings::Load() {
  PRT_PROFILING_BEGIN(loading);
  {
    const bool is_genesis = !fs::exists(GetPath());
    PublishLoadingSettingsEvent();
    rocksdb::Options options{};
    options.create_if_missing = true;
    DB* db = nullptr;
    const auto status = DB::Open(options, GetPath(), &db);
    LOG_IF(FATAL, !status.ok()) << "failed to open settings database: " << status.ToString();
    ASSERT(db);
    db_ = db;
    if (is_genesis)
      PublishGenerateSettingsEvent();
    PublishSettingsLoadedEvent();
  }
  PRT_PROFILING_END(loading);
  DVLOG(1) << "settings loaded in " << std::chrono::duration_cast<std::chrono::milliseconds>(loading_duration).count()
           << "ms.";
}
}  // namespace prt