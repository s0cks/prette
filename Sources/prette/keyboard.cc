#include "prette/keyboard.h"

#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>

#include <filesystem>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/lua.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
static Keyboard* keyboard_ = nullptr;
static KeyboardEventSubject all_events{};

static inline void PublishEvent(KeyboardEvent* event) {
  ASSERT(event);
  const auto& subscriber = all_events.get_subscriber();
  return subscriber.on_next(event);
}

auto OnKeyboardEvent() -> KeyboardEventObservable {
  return all_events.get_observable();
}

auto KeyboardInitEvent::ToString() const -> std::string {
  ToStringHelper<KeyboardInitEvent> helper{};
  helper.AddFieldPtr("keyboard", GetKeyboard());
  return helper;
}

auto KeyStateEvent::GetKey() const -> const char* {
  const auto name = glfwGetKeyName(GetCode(), GetScanCode());
  return name ? name : "";
}

void KeyStateEvent::ToTable(lua_State* L) const {
  ASSERT(L);
  KeyboardEvent::ToTable(L);
  lua_pushstring(L, IsPressed() ? "pressed" : IsReleased() ? "released" : IsRepeat() ? "repeat" : "unknown");
  lua_setfield(L, -2, "state");

  lua_pushstring(L, GetKey());
  lua_setfield(L, -2, "key");

  lua_pushnumber(L, GetMods());
  lua_setfield(L, -2, "mods");
}

auto KeyStateEvent::ToString() const -> std::string {
  ToStringHelper<KeyStateEvent> helper{};
  helper.AddFieldPtr("keyboard", GetKeyboard());
  helper.AddField("key", GetKey());
  helper.AddFieldRef("state", GetState());
  helper.AddFieldRef("mods", GetMods());
  helper.AddFieldRef("scancode", GetScanCode());
  helper.AddFieldRef("code", GetCode());
  return helper;
}

auto KeyboardDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<KeyboardDestroyedEvent>{};
}

auto Keyboard::GetKeyboard(GLFWwindow* handle) -> Keyboard* {
  const auto window = Window::Get(handle);
  ASSERT(window && window->HasKeyboard());
  return window->GetKeyboard();
}

void Keyboard::OnKey(GLFWwindow* handle, int key, int scancode, int action, int mods) {
  ASSERT(handle);
  const auto keyboard = GetKeyboard(handle);
  ASSERT(keyboard);
  keyboard->Publish<KeyStateEvent>(keyboard, key, scancode, action, mods);
}

Keyboard::Keyboard(Window* owner) :
  owner_(owner) {
  ASSERT(owner_);
  glfwSetKeyCallback(owner_->GetHandle(), &OnKey);
}

Keyboard::~Keyboard() {
  glfwSetKeyCallback(owner_->GetHandle(), nullptr);
}

auto Keyboard::ToString() const -> std::string {
  return ToStringHelper<Keyboard>{};
}

void Keyboard::PublishEvent(KeyboardEvent* event) const {
  ASSERT(event);
  prt::PublishEvent(event);
  return EventSourceTemplate<KeyboardEvent>::PublishEvent(event);
}

auto Keyboard::GetKey(const int code) const -> KeyState {
  return {code, glfwGetKey(GetOwner()->GetHandle(), code)};
}

static inline auto SetKeyboard(Keyboard* rhs) -> Keyboard* {
  ASSERT(rhs);
  keyboard_ = rhs;
  return rhs;
}

auto Keyboard::IsInitialized() -> bool {
  return keyboard_ != nullptr;
}

auto Keyboard::New(Window* owner) -> Keyboard* {
  ASSERT(owner);
  const auto keyboard = new Keyboard(owner);
  ASSERT(keyboard);
  keyboard->Publish<KeyboardInitEvent>(keyboard);
  return keyboard;
}

auto Keyboard::Get() -> Keyboard* {
  ASSERT(keyboard_);
  return keyboard_;
}

void Keyboard::Init(Window* window) {
  ASSERT(window);
  const auto keyboard = SetKeyboard(Keyboard::New(window));
  ASSERT(keyboard);
  window->SetKeyboard(keyboard);
}

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

void Keyboard::InitLua(lua_State* L) {
  ASSERT(L);
  DLOG(INFO) << "initializing lua bindings...";
  lua_newtable(L);

  luaL_newmetatable(L, "Keyboard");
  lua_pushcfunction(L, &lua_keyboard_tostring);
  lua_setfield(L, -2, "__tostring");
  lua_setmetatable(L, -2);

  luaL_setfuncs(L, kKeyboardLib, 0);
  lua_setglobal(L, "Keyboard");
}
}  // namespace prt