#include "prette/keyboard/keyboard.h"

#include <string>

#include "prette/assertions.h"
#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/lua.h"
#include "prette/to_string.h"
#include "prette/window/window.h"

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

#ifdef PRT_ENABLE_LUA
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
#endif  // PRT_ENABLE_LUA

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

#ifdef PRT_GLFW
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

auto Keyboard::GetKey(const int code) const -> KeyState {
  return {code, glfwGetKey(GetOwner()->GetHandle(), code)};
}

auto KeyStateEvent::GetKey() const -> const char* {
  const auto name = glfwGetKeyName(GetCode(), GetScanCode());
  return name ? name : "";
}
#endif  // PRT_GLFW

auto Keyboard::ToString() const -> std::string {
  return ToStringHelper<Keyboard>{};
}

void Keyboard::PublishEvent(KeyboardEvent* event) const {
  ASSERT(event);
  prt::PublishEvent(event);
  return EventSourceTemplate<KeyboardEvent>::PublishEvent(event);
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
}  // namespace prt