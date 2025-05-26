#include "prette/keyboard/keyboard.h"
#ifdef PRT_ENABLE_LUA

#include "prette/common.h"
#include "prette/lua.h"

namespace prt {
class KeyboardModule : LuaModule {
  friend class Keyboard;
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

#define LUA_KEYBOARD_F(Name) LUA_F(keyboard_##Name)

LUA_KEYBOARD_F(tostring) {
  if (!Keyboard::IsInitialized()) {
    lua_pushstring(L, "keyboard is not initialized!");
    return 1;
  }
  const auto value = Keyboard::Get()->ToString();
  lua_pushstring(L, value.c_str());
  return 1;
}

LUA_KEYBOARD_F(onEvent) {
  OnKeyboardEvent().subscribe(CreateSubscriber<KeyboardEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_F(keyboard_on##Name##Event) {                                \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_KEYBOARD_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

static inline auto CheckKeyCode(lua_State* L, const int index) -> int {
  luaL_checktype(L, index, LUA_TNUMBER);
  return lua_tonumber(L, index);
}

LUA_KEYBOARD_F(isPressed) {
  const auto code = CheckKeyCode(L, 1);
  const auto keyboard = Keyboard::Get();
  ASSERT(keyboard);
  const auto state = keyboard->GetKey(code);
  lua_pushboolean(L, state.IsPressed());
  return 1;
}

LUA_KEYBOARD_F(isReleased) {
  const auto code = CheckKeyCode(L, 1);
  const auto keyboard = Keyboard::Get();
  ASSERT(keyboard);
  const auto state = keyboard->GetKey(code);
  lua_pushboolean(L, state.IsReleased());
  return 1;
}

LUA_KEYBOARD_F(isRepeat) {
  const auto code = CheckKeyCode(L, 1);
  const auto keyboard = Keyboard::Get();
  ASSERT(keyboard);
  const auto state = keyboard->GetKey(code);
  lua_pushboolean(L, state.IsRepeat());
  return 1;
}

LUA_KEYBOARD_F(onPressed) {
  OnKeyPressed(CheckKeyCode(L, 1)).subscribe(CreateSubscriber<KeyStateEvent>(L, 2));
  return 0;
}

LUA_KEYBOARD_F(onReleased) {
  OnKeyReleased(CheckKeyCode(L, 1)).subscribe(CreateSubscriber<KeyStateEvent>(L, 2));
  return 0;
}

#undef LUA_KEYBOARD_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kKeyboardLib[] = {
#define LUA_KEYBOARD_F(Name) \
  {.name = #Name, .func = &lua_keyboard_##Name }

LUA_KEYBOARD_F(onEvent),
LUA_KEYBOARD_F(onPressed),
LUA_KEYBOARD_F(onReleased),
LUA_KEYBOARD_F(isPressed),
LUA_KEYBOARD_F(isReleased),
LUA_KEYBOARD_F(isRepeat),
#define DEFINE_ON_EVENT(Name) \
  LUA_KEYBOARD_F(on##Name##Event),
  FOR_EACH_KEYBOARD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#undef LUA_KEYBOARD_F
};
// clang-format on

void KeyboardModule::Init(lua_State* L) {
  ASSERT(L);
  LuaModule::InitModule(L, "keyboard", "Keyboard", kKeyboardLib, &lua_keyboard_tostring);
}

void Keyboard::InitLua(lua_State* L) {
  KeyboardModule::Init(L);
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA