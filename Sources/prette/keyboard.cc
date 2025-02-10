#include "prette/keyboard.h"

#include <GLFW/glfw3.h>

#include "prette/common.h"
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

auto KeyboardCreatedEvent::ToString() const -> std::string {
  ToStringHelper<KeyboardCreatedEvent> helper{};
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

auto Keyboard::New(Window* owner) -> Keyboard* {
  ASSERT(owner);
  const auto keyboard = new Keyboard(owner);
  ASSERT(keyboard);
  keyboard->PublishKeyboardCreatedEvent();
  return keyboard;
}

auto Keyboard::Get() -> Keyboard* {
  ASSERT(keyboard_);
  return keyboard_;
}

auto Keyboard::Init(Window* window) -> Keyboard* {
  ASSERT(window);
  const auto keyboard = SetKeyboard(Keyboard::New(window));
  ASSERT(keyboard);
  window->SetKeyboard(keyboard);
  return keyboard;
}

LUA_F(keyboard_onEvent) {
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

#define LUA_KEYBOARD_F(Name) LUA_F(keyboard_##Name)

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
#define DEFINE_ON_EVENT(Name) \
  LUA_KEYBOARD_F(on##Name##Event),
  FOR_EACH_KEYBOARD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#undef LUA_KEYBOARD_F
};
// clang-format on

void Keyboard::InitLua(lua_State* L) {
  ASSERT(L);
  lua_newtable(L);
  luaL_setfuncs(L, kKeyboardLib, 0);
  lua_setglobal(L, "Keyboard");
}
}  // namespace prt