#include "prette/settings/settings_lua_bindings.h"
#ifdef PRT_ENABLE_LUA

#include <fmt/format.h>

#include "prette/common.h"
#include "prette/lua.h"
#include "prette/settings/settings.h"
#include "prette/settings/settings_event.h"

namespace prt {
#define LUA_SETTINGS_F(Name) LUA_F(settings_##Name)

LUA_SETTINGS_F(onEvent) {
  OnSettingsEvent(CreateSubscriber<SettingsEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)              \
  LUA_SETTINGS_F(on##Name##Event) {             \
    On##Name(CreateSubscriber<Name##Event>(L)); \
    return 0;                                   \
  }
FOR_EACH_SETTINGS_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_SETTINGS_F

// clang-format off
// NOLINTNEXTLINE
DEFINE_LUALIB(Settings) {
#define LUA_SETTINGS_F(Name) \
  {.name = #Name, .func = &lua_settings_##Name }

LUA_SETTINGS_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_SETTINGS_F(on##Name##Event),
  FOR_EACH_SETTINGS_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#undef LUA_SETTINGS_F
};
// clang-format on

LUA_F(settings_index) {
  const auto index = luaL_checkstring(L, 2);
  DLOG(INFO) << "index: " << index;
  lua_pushstring(L, fmt::format("Hello {}", index).c_str());
  return 1;
}

void SettingsModule::Init(lua_State* L) {
  DVLOG(10) << "initializing lua bindings...";
  lua_newtable(L);

  luaL_newmetatable(L, "Settings");

  lua_pushcfunction(L, &lua_settings_index);
  lua_setfield(L, -2, "__index");

  lua_setmetatable(L, -2);

  luaL_setfuncs(L, kSettingsLib, 0);
  lua_setglobal(L, "settings");
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA