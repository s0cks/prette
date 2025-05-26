#ifndef PRT_RENDERER_LUA_BINDINGS_H
#define PRT_RENDERER_LUA_BINDINGS_H

#ifdef PRT_ENABLE_LUA

#include "prette/lua.h"

namespace prt {
class RendererModule : LuaModule {
  friend class Renderer;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};
}  // namespace prt

#endif  // PRT_ENABLE_LUA

#endif  // PRT_RENDERER_LUA_BINDINGS_H
