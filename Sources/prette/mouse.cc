#include "prette/mouse.h"

#include <GLFW/glfw3.h>
#include <lauxlib.h>
#include <lua.h>

#include "prette/lua.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
static MouseEventSubject events_{};
static Mouse* mouse_ = nullptr;

void Mouse::PublishEvent(MouseEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

auto OnMouseEvent() -> MouseEventObservable {
  return events_.get_observable();
}

auto MouseCreatedEvent::ToString() const -> std::string {
  ToStringHelper<MouseCreatedEvent> helper{};
  return helper;
}

auto MouseMotionEvent::ToString() const -> std::string {
  ToStringHelper<MouseMotionEvent> helper{};
  helper.AddFieldPtr("mouse", GetMouse());
  helper.AddField("direction", glm::to_string(GetDirection()));
  return helper;
}

auto MouseDestroyedEvent::ToString() const -> std::string {
  ToStringHelper<MouseDestroyedEvent> helper{};
  return helper;
}

Mouse::Mouse(Window* owner) :
  owner_(owner) {
  ASSERT(owner_);
  glfwSetMouseButtonCallback(owner_->GetHandle(), &OnMouseButton);
  glfwSetCursorPosCallback(owner_->GetHandle(), &OnMouseMotion);
}

Mouse::~Mouse() {
  ASSERT(owner_);
  glfwSetMouseButtonCallback(owner_->GetHandle(), nullptr);
  glfwSetCursorPosCallback(owner_->GetHandle(), nullptr);
}

auto Mouse::IsInitialized() -> bool {
  return mouse_ != nullptr;
}

static inline auto SetMouse(Mouse* rhs) -> Mouse* {
  ASSERT(rhs);
  mouse_ = rhs;
  return rhs;
}

auto Mouse::GetPos() const -> glm::dvec2 {
  glm::dvec2 pos;
  glfwGetCursorPos(GetOwner()->GetHandle(), &pos.x, &pos.y);
  return pos;
}

auto Mouse::IsPressed(const int button) const -> bool {
  return glfwGetMouseButton(GetOwner()->GetHandle(), button);
}

auto Mouse::New(Window* window) -> Mouse* {
  const auto mouse = new Mouse(window);
  ASSERT(mouse);
  Publish<MouseCreatedEvent>(mouse);
  return mouse;
}

auto Mouse::Init(Window* window) -> Mouse* {
  ASSERT(window);
  ASSERT(!IsInitialized());
  const auto mouse = SetMouse(Mouse::New(window));
  ASSERT(mouse);
  window->SetMouse(mouse);
  return mouse;
}

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
static const struct luaL_Reg kMouseLib[] = {
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

void Mouse::InitLua(lua_State* L) {
  ASSERT(L);
  DLOG(INFO) << "initializing lua bindings....";
  lua_newtable(L);
  luaL_setfuncs(L, kMouseLib, 0);
  lua_setglobal(L, "Mouse");
}

auto Mouse::Get() -> Mouse* {
  ASSERT(IsInitialized());
  return mouse_;
}
}  // namespace prt