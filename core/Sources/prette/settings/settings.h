#ifndef PRT_SETTINGS_H
#define PRT_SETTINGS_H

#include <optional>
#include <ostream>
#include <string>
#include <utility>

// IWYU pragma: begin_exports
#include <rocksdb/db.h>
#include <rocksdb/env.h>
#include <rocksdb/options.h>
#include <rocksdb/status.h>
// IWYU pragma: end_exports

#include "prette/common.h"
#include "prette/resolution.h"
#include "prette/rx.h"
#include "prette/settings/setting_type.h"
#include "prette/settings/settings_event.h"

namespace prt {
auto GetSettingsEventObservable() -> SettingsEventObservable;

template <typename... ArgN>
static inline auto OnSettingsEvent(ArgN... args) -> rx::composite_subscription {
  return GetSettingsEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                                                               \
  template <typename... ArgN>                                                               \
  static inline auto On##Name##Event()->Name##EventObservable {                             \
    return GetSettingsEventObservable().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                                         \
  template <typename... ArgN>                                                               \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {                   \
    return On##Name##Event().subscribe(args...);                                            \
  }
FOR_EACH_SETTINGS_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

static inline auto operator<<(std::ostream& stream, const rocksdb::Status status) -> std::ostream& {
  return stream << status.ToString();
}

class Settings {
 public:
  using DB = rocksdb::DB;
  using Key = std::string;

  friend class SettingsSystem;

 private:
  static void PublishEvent(SettingsEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

#define DEFINE_PUBLISH(Name)                              \
  template <typename... Args>                             \
  static inline void Publish##Name##Event(Args... args) { \
    return Publish<Name##Event>(args...);                 \
  }
  FOR_EACH_SETTINGS_EVENT(DEFINE_PUBLISH)
#undef DEFINE_PUBLISH
 private:
  fs::path path_;
  DB* db_ = nullptr;

  explicit Settings(fs::path path);

  inline auto GetDB() const -> DB* {
    return db_;
  }

  void Save();
  void Load();

 public:
  ~Settings();

  auto GetPath() const -> const fs::path& {
    return path_;
  }

  template <typename V, SettingType<V> Setting>
  inline auto GetSetting() -> std::optional<V> {
    std::string value{};
    rocksdb::ReadOptions options{};
    const auto status = GetDB()->Get(options, Setting::kSettingName, &value);
    if (status.IsNotFound())
      return std::nullopt;
    LOG_IF(FATAL, !status.ok()) << "failed to get " << Setting::kTypeName << " name " << Setting::kSettingName
                                << " in settings: " << status.ToString();
    Setting setting{};
    return setting((const uint8_t*)value.data(), value.size());  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

  template <typename V, SettingType<V> Setting>
  inline auto PutSetting(const rocksdb::WriteOptions options, const V value) -> rocksdb::Status {
    const auto value_data = (std::string)value;
    const auto status = GetDB()->Put(options, Setting::kSettingName, value_data);
    DLOG_IF(ERROR, !status.ok()) << "failed to put " << Setting::kTypeName << " name " << Setting::kSettingName
                                 << " in settings: " << status.ToString();
    return status;
  }

  template <typename V, SettingType<V> Setting>
  inline auto PutSetting(const V value) -> rocksdb::Status {
    rocksdb::WriteOptions options{};
    options.sync = true;
    return PutSetting<V, Setting>(std::move(options), std::move(value));
  }

#define _DEFINE_SETTINGS_FUNCTIONS_FOR_TYPE(Type, TypeName)                         \
  template <SettingType<Type> Setting>                                              \
  inline auto Get##TypeName()->std::optional<Type> {                                \
    return GetSetting<Type, Setting>();                                             \
  }                                                                                 \
  template <SettingType<Type> Setting>                                              \
  inline auto Put##TypeName(const Type value)->rocksdb::Status {                    \
    return PutSetting<Type, Setting>(std::move(value));                             \
  }                                                                                 \
  template <SettingType<Type> Setting>                                              \
  inline auto GetOrCreate##TypeName(const Type default_value)->Type {               \
    const auto current_value = Get##TypeName<Setting>();                            \
    if (current_value)                                                              \
      return (*current_value);                                                      \
    const auto status = Put##TypeName<Setting>(std::move(default_value));           \
    LOG_IF(ERROR, !status.ok()) << "failed to put " << #TypeName << ": " << status; \
    return default_value;                                                           \
  }
#define DEFINE_SETTINGS_FUNCTIONS_FOR_TYPE(Type) _DEFINE_SETTINGS_FUNCTIONS_FOR_TYPE(Type, Type)

  _DEFINE_SETTINGS_FUNCTIONS_FOR_TYPE(bool, Bool);
  DEFINE_SETTINGS_FUNCTIONS_FOR_TYPE(Resolution);

#undef _DEFINE_SETTINGS_FUNCTIONS_FOR_TYPE
#undef DEFINE_SETTINGS_FUNCTIONS_FOR_TYPE
};
}  // namespace prt

#endif  // PRT_SETTINGS_H
