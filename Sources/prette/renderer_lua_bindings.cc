#include "prette/renderer.h"

#ifdef PRETTE_ENABLE_LUA
#include "prette/lua.h"
#include "prette/renderer_event.h"

namespace prt {

class RendererModule : LuaModule {
  friend class Renderer;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

#define LUA_RENDERER_F(Name) LUA_F(renderer_##Name)

LUA_RENDERER_F(onEvent) {
  OnRendererEvent().subscribe(CreateSubscriber<RendererEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_RENDERER_F(on##Name##Event) {                                \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_RENDERER_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kRendererLib[] = {
#define LUA_RENDERER_F(Name) \
  { .name = #Name, .func = &lua_renderer_##Name }

  LUA_RENDERER_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_RENDERER_F(on##Name##Event),
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_RENDERER_F
};
// clang-format on

void RendererModule::Init(lua_State* L) {
  InitModule(L, "renderer", "Renderer", kRendererLib);
}

void Renderer::InitLua(lua_State* L) {
  return RendererModule::Init(L);
}
}  // namespace prt

#endif  // PRETTE_ENABLE_LUA