#ifndef PRT_SETTINGS_H
#define PRT_SETTINGS_H

#include <rocksdb/db.h>

#include <optional>

#include "prette/resolution.h"
#include "prette/settings_event.h"

namespace prt {
auto OnSettingsEvent() -> SettingsEventObservable;

#define DEFINE_ON_EVENT(Name)                                                    \
  static inline auto On##Name##Event()->Name##EventObservable {                  \
    return OnSettingsEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_SETTINGS_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

using SettingsDB = rocksdb::DB;

class LuaState;
class Settings {
  friend class LuaState;

 private:
  static void InitLua(lua_State* L);

 public:
  static void Load();
  static void Save();
  static void Init();
  static auto IsInitialized() -> bool;
  static auto GetDB() -> SettingsDB*;

  static void PutResolution(const std::string k, const Resolution resolution);
  static auto GetResolution(const std::string k) -> std::optional<Resolution>;

  static auto GetSettingsPath() -> fs::path;
};
}  // namespace prt

#endif  // PRT_SETTINGS_H
