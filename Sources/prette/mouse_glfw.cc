#include "prette/platform.h"
#ifdef PRT_GLFW

#include "prette/mouse.h"
#include "prette/window.h"

namespace prt {
void Mouse::OnMouseButton(GLFWwindow* handle, int button, int action, int mods) {
  ASSERT(handle);
  const auto window = Window::Get(handle);
  ASSERT(window);
  const auto mouse = window->GetMouse();
  ASSERT(mouse);
}

void Mouse::OnMouseMotion(GLFWwindow* handle, double xPos, double yPos) {
  ASSERT(handle);
  const auto window = Window::Get(handle);
  ASSERT(window);
  const auto mouse = window->GetMouse();
  ASSERT(mouse);
  const auto new_pos = glm::vec2(xPos, yPos);
  const auto delta = new_pos - mouse->GetPreviousPos();
  Publish<MouseMotionEvent>(mouse, delta);
  mouse->previous_pos_ = new_pos;
}
}  // namespace prt

#endif  // PRT_GLFW