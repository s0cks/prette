#include "prette/mouse/mouse_button.h"

#include "prette/window/window.h"

namespace prt::mouse {
  MouseButton::State MouseButton::GetState(GLFWwindow* handle) const {
    PRT_ASSERT(handle);
    return static_cast<MouseButton::State>(glfwGetMouseButton(handle, GetCode()));
  }
}