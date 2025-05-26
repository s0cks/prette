#ifndef PRT_WORLD_LUA_BINDINGS_H
#define PRT_WORLD_LUA_BINDINGS_H

#ifdef PRT_ENABLE_LUA

#include "prette/lua.h"

namespace prt {
class WorldModule : public LuaModule {
  friend class LuaState;
  friend class WorldManager;
  static void Init(lua_State* L);
};
}  // namespace prt

#endif  // PRT_ENABLE_LUA

#endif  // PRT_WORLD_LUA_BINDINGS_H
