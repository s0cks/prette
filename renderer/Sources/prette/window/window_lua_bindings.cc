#include "prette/window/window.h"
#ifdef PRT_ENABLE_LUA

#include "prette/common.h"
#include "prette/lua.h"
#include "prette/window/window_event.h"

namespace prt {
class WindowModule : LuaModule {
  friend class Window;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

#define LUA_WINDOW_F(Name) LUA_F(window_##Name)

LUA_WINDOW_F(onEvent) {
  OnWindowEvent().subscribe(CreateSubscriber<WindowEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_WINDOW_F(on##Name##Event) {                                  \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_WINDOW_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_WINDOW_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kWindowLib[] = {
#define LUA_WINDOW_F(Name) \
  { .name = #Name, .func = &lua_window_##Name }

  LUA_WINDOW_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_WINDOW_F(on##Name##Event),
FOR_EACH_WINDOW_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_WINDOW_F
};
// clang-format on

void WindowModule::Init(lua_State* L) {
  ASSERT(L);
  InitModule(L, "window", "Window", kWindowLib);
}

void Window::InitLua(lua_State* L) {
  return WindowModule::Init(L);
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA