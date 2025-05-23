#include "prette/mouse.h"
#ifdef PRT_GLFW

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/glm.h"
#include "prette/gui_viewport.h"
#include "prette/window.h"

namespace prt {
Mouse::Mouse(Window* owner) :
  owner_(owner) {
  ASSERT(owner_);
  glfwSetMouseButtonCallback(owner_->GetHandle(), &OnMouseButton);
  glfwSetCursorPosCallback(owner_->GetHandle(), &OnMouseMotion);
  glfwSetScrollCallback(owner_->GetHandle(), &OnMouseScroll);
}

Mouse::~Mouse() {
  ASSERT(owner_);
  glfwSetMouseButtonCallback(owner_->GetHandle(), nullptr);
  glfwSetCursorPosCallback(owner_->GetHandle(), nullptr);
}

auto Mouse::GetPos() const -> glm::dvec2 {
  glm::dvec2 pos;
  glfwGetCursorPos(GetOwner()->GetHandle(), &pos.x, &pos.y);
  if (GuiViewport::IsInitialized())
    pos -= GuiViewport::Get()->GetPos();
  return pos;
}

auto Mouse::IsPressed(const int button) const -> bool {
  return glfwGetMouseButton(GetOwner()->GetHandle(), button);
}

auto Mouse::Unwrap(GLFWwindow* handle) -> Mouse* {
  ASSERT(handle);
  const auto window = Window::Get(handle);
  ASSERT(window);
  return window->GetMouse();
}

void Mouse::OnMouseButton(GLFWwindow* handle, int button, int action, int mods) {
  const auto mouse = Unwrap(handle);
  MouseButtonState state(button, action, mods);
  if (state.IsButton1()) {
    if (state.IsPressed()) {
      mouse->StartDragging(button, mods);
    } else if (state.IsReleased()) {
      mouse->StopDragging();
    }
  }
  MouseButtonStateEvent event(mouse, state);
  PublishEvent(&event);
}

void Mouse::OnMouseScroll(GLFWwindow* handle, double xOffset, double yOffset) {
  const auto delta = ScrollDelta(xOffset, yOffset);
  Publish<ScrollEvent>(delta);
}

void Mouse::OnMouseMotion(GLFWwindow* handle, double xPos, double yPos) {
  const auto mouse = Unwrap(handle);
  const auto new_pos = glm::vec2(xPos, yPos);
  const auto delta = new_pos - mouse->GetPreviousPos();
  Publish<MouseMotionEvent>(mouse, delta);
  mouse->previous_pos_ = new_pos;
}
}  // namespace prt

#endif  // PRT_GLFW