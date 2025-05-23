#include "prette/lua.h"
#include "prette/world_manager.h"
#ifdef PRETTE_ENABLE_LUA

#include "prette/common.h"
#include "prette/world.h"
#include "prette/world_event.h"

namespace prt {
#define LUA_WORLD_F(Name) LUA_F(world_##Name)

LUA_WORLD_F(getName) {
  ASSERT(IsWorldInitialized());
  lua_pushstring(L, GetWorld()->GetName().c_str());
  return 1;
}

LUA_WORLD_F(onEvent) {
  OnWorldEvent().subscribe(CreateSubscriber<WorldEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT(Name)                                      \
  LUA_WORLD_F(on##Name##Event) {                                   \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_WORLD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#undef LUA_WORLD_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kWorldLib[] = {
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

class WorldModule : public LuaModule {
  friend class World;
  friend class LuaState;

  static void Init(lua_State* L) {
    InitModule(L, "world", "World", kWorldLib);
  }
};

void World::InitLua(lua_State* L) {
  WorldModule::Init(L);
}
}  // namespace prt

#endif  // PRETTE_ENABLE_LUA