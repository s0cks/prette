#include "prette/world/world_lua_bindings.h"

#ifdef PRT_ENABLE_LUA

#include "prette/assertions.h"
#include "prette/lua.h"
#include "prette/world/world.h"
#include "prette/world/world_event.h"
#include "prette/world/world_manager.h"

namespace prt {
#define LUA_WORLD_F(Name) LUA_F(world_##Name)

LUA_WORLD_F(getName) {
  ASSERT(IsWorldInitialized());
  lua_pushstring(L, GetWorld()->GetName().c_str());
  return 1;
}

LUA_WORLD_F(onEvent) {
  OnWorldEvent(CreateSubscriber<WorldEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT(Name)                   \
  LUA_WORLD_F(on##Name##Event) {                \
    On##Name(CreateSubscriber<Name##Event>(L)); \
    return 0;                                   \
  }
FOR_EACH_WORLD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#undef LUA_WORLD_F

// clang-format off
// NOLINTNEXTLINE
DEFINE_LUALIB(World) {
#define LUA_WORLD_F(Name) \
  {.name = #Name, .func = &lua_world_##Name }

  LUA_WORLD_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_WORLD_F(on##Name##Event),
  FOR_EACH_WORLD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_WORLD_F
};
// clang-format on

void WorldModule::Init(lua_State* L) {
  InitModule(L, "world", "World", kWorldLib);
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA