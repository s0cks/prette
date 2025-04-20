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

auto Mouse::Get() -> Mouse* {
  ASSERT(IsInitialized());
  return mouse_;
}

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
}  // namespace prt