#include "prette/gfx_driver.h"
#ifdef PRT_ENABLE_LUA

#include "prette/common.h"
#include "prette/gfx_driver_event.h"
#include "prette/lua.h"

namespace prt {
class DriverModule : LuaModule {
  friend class DriverBase;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

#define LUA_DRIVER_F(Name) LUA_F(driver_##Name)

LUA_DRIVER_F(onEvent) {
  OnDriverEvent().subscribe(CreateSubscriber<DriverEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_DRIVER_F(on##Name##Event) {                                  \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_DRIVER_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_DRIVER_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kDriverLib[] = {
#define LUA_DRIVER_F(Name) \
  { .name = #Name, .func = &lua_driver_##Name }

  LUA_DRIVER_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_DRIVER_F(on##Name##Event),
FOR_EACH_DRIVER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_DRIVER_F
};
// clang-format on

void DriverModule::Init(lua_State* L) {
  ASSERT(L);
  InitModule(L, "driver", "Driver", kDriverLib);
}

void DriverBase::InitLua(lua_State* L) {
  return DriverModule::Init(L);
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA