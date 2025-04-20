#include "prette/settings.h"

#include <fmt/format.h>
#include <glog/logging.h>
#include <rocksdb/env.h>
#include <rocksdb/options.h>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/lua.h"
#include "prette/settings_event.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"
#include "prette/window.h"
#include "prette/window_event.h"

namespace prt {
static ThreadLocal<SettingsDB> db_{};
static SettingsEventSubject events_{};

auto Settings::GetSettingsPath() -> fs::path {
  return fs::current_path() / "settings";
}

static inline void PublishEvent(SettingsEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

auto OnSettingsEvent() -> SettingsEventObservable {
  return events_.get_observable();
}

void Settings::Save() {
  Publish<SettingsSavedEvent>();
}

void Settings::Load() {
  rocksdb::Options options{};
  options.create_if_missing = true;
  SettingsDB* db = nullptr;
  const auto status = rocksdb::DB::Open(options, GetSettingsPath(), &db);
  LOG_IF(FATAL, !status.ok()) << "failed to open settings database: " << status.ToString();
  ASSERT(db);
  db_.Set(db);
  Publish<SettingsLoadedEvent>();
}

void Settings::Init() {
  const bool first_init = !fs::exists(GetSettingsPath());
  Load();
  if (first_init) {
    PutResolution("resolution", Resolution(512, 512));
    OnPostInitEvent().subscribe([](PostInitEvent* event) {
      const auto window = GetAppWindow();
      ASSERT(window);
      const auto fbsize = window->GetFramebufferSize();
      PutResolution("resolution", Resolution(fbsize.width(), fbsize.height()));
    });
  }
}

auto Settings::IsInitialized() -> bool {
  return db_.Get() != nullptr;
}

auto Settings::GetDB() -> SettingsDB* {
  ASSERT(IsInitialized());
  return db_.Get();
}

void Settings::PutResolution(const std::string k, const Resolution v) {
  rocksdb::WriteOptions options{};
  const auto status = GetDB()->Put(options, k, v);
  LOG_IF(FATAL, !status.ok()) << "failed to put Resolution " << k << " (" << v << ") in settings: " << status.ToString();
}

auto Settings::GetResolution(const std::string k) -> std::optional<Resolution> {
  std::string value{};
  rocksdb::ReadOptions options{};
  const auto status = GetDB()->Get(options, k, &value);
  if (status.IsNotFound())
    return std::nullopt;
  LOG_IF(FATAL, !status.ok()) << "failed to get Resolution name " << k << " in settings: " << status.ToString();
  return {Resolution(*((uint64_t*)value.data()))};
}
}  // namespace prt