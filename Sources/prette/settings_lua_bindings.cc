#include <fmt/format.h>

#include "prette/lua.h"
#include "prette/settings.h"

namespace prt {
#define LUA_SETTINGS_F(Name) LUA_F(settings_##Name)

LUA_SETTINGS_F(load) {
  Settings::Load();
  return 0;
}

LUA_SETTINGS_F(save) {
  Settings::Save();
  return 0;
}

LUA_SETTINGS_F(onEvent) {
  OnSettingsEvent().subscribe(CreateSubscriber<SettingsEvent>(L));
  return 0;
}

LUA_SETTINGS_F(getResolution) {
  const auto key = luaL_checkstring(L, 1);
  const auto resolution = Settings::GetResolution(key);
  if (!resolution) {
    luaL_error(L, "failed to find Resolution %s in Settings.", key);
    return 1;
  }

  resolution->ToTable(L);
  return 1;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_SETTINGS_F(on##Name##Event) {                                \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_SETTINGS_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_SETTINGS_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kSettingsLib[] = {
#define LUA_SETTINGS_F(Name) \
  {.name = #Name, .func = &lua_settings_##Name }

LUA_SETTINGS_F(save),
LUA_SETTINGS_F(load),
LUA_SETTINGS_F(onEvent),
LUA_SETTINGS_F(getResolution),

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

class SettingsModule {
  friend class Settings;
  friend class LuaState;

 private:
  static void Init(lua_State* L) {
    DVLOG(10) << "initializing lua bindings...";
    lua_newtable(L);

    luaL_newmetatable(L, "Settings");

    lua_pushcfunction(L, &lua_settings_index);
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, -2);

    luaL_setfuncs(L, kSettingsLib, 0);
    lua_setglobal(L, "settings");
  }
};

void Settings::InitLua(lua_State* L) {
  SettingsModule::Init(L);
}
}  // namespace prt