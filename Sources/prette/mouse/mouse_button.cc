#include "prette/mouse/mouse_button.h"

#include "prette/window/window.h"

namespace prt::mouse {
  auto MouseButton::GetState(GLFWwindow* handle) const -> MouseButton::State {
    PRT_ASSERT(handle);
    return static_cast<MouseButton::State>(glfwGetMouseButton(handle, GetCode()));
  }
}