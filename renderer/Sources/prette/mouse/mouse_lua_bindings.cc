#include "prette/mouse/mouse.h"
#ifdef PRT_ENABLE_LUA

#include <glog/logging.h>

#include "prette/lua.h"
#include "prette/mouse/mouse_event.h"

namespace prt {
class MouseModule : LuaModule {
  friend class Mouse;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

#define LUA_MOUSE_F(Name) LUA_F(mouse_##Name)

LUA_MOUSE_F(onEvent) {
  OnMouseEvent().subscribe(CreateSubscriber<MouseEvent>(L));
  return 0;
}

LUA_MOUSE_F(onButton) {
  luaL_checktype(L, 1, LUA_TNUMBER);
  const auto button = lua_tointeger(L, -1);
  OnMouseButton(static_cast<int>(button)).subscribe(CreateSubscriber<MouseButtonStateEvent>(L, 2));
  return 0;
}

LUA_MOUSE_F(onButtonPressed) {
  luaL_checktype(L, 1, LUA_TNUMBER);
  const auto button = lua_tointeger(L, -1);
  OnMousePressed(static_cast<int>(button)).subscribe(CreateSubscriber<MouseButtonStateEvent>(L, 2));
  return 0;
}

LUA_MOUSE_F(onButtonReleased) {
  luaL_checktype(L, 1, LUA_TNUMBER);
  const auto button = lua_tointeger(L, -1);
  OnMouseReleased(static_cast<int>(button)).subscribe(CreateSubscriber<MouseButtonStateEvent>(L, 2));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_MOUSE_F(on##Name##Event) {                                   \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_MOUSE_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_MOUSE_F

// clang-format off
// NOLINTNEXTLINE
DEFINE_LUALIB(Mouse) {
#define LUA_MOUSE_F(Name) \
  {.name = #Name, .func = &lua_mouse_##Name }

LUA_MOUSE_F(onEvent),
LUA_MOUSE_F(onButton),
LUA_MOUSE_F(onButtonPressed),
LUA_MOUSE_F(onButtonReleased),
#define DEFINE_ON_EVENT(Name) \
  LUA_MOUSE_F(on##Name##Event),
  FOR_EACH_MOUSE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#undef LUA_KEYBOARD_F
};
// clang-format on

void MouseModule::Init(lua_State* L) {
  DVLOG(1) << "initializing lua bindings....";
  InitModule(L, "mouse", "Mouse", kMouseLib);
}

void Mouse::InitLua(lua_State* L) {
  return MouseModule::Init(L);
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA