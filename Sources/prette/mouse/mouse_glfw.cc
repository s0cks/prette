#include "prette/mouse/mouse.h"
#ifdef PRT_GLFW

#include "prette/transform.h"
#include "prette/engine/engine.h"
#include "prette/window/window.h"

namespace prt::mouse {
  static inline window::Window::Handle*
  GetAppWindowHandle() {
    const auto window = window::GetAppWindow();
    PRT_ASSERT(window);
    return window->GetHandle();
  }

  MouseButton::State Mouse::GetButtonState(const MouseButton::Code code) const {
    const auto window = GetAppWindowHandle();
    PRT_ASSERT(window);
    const auto state = glfwGetMouseButton(window, code);
    switch(state) {
      case GLFW_PRESS:
        return MouseButton::kPressed;
      case GLFW_RELEASE:
        return MouseButton::kReleased;
      case GLFW_REPEAT:
      default:
        DLOG(WARNING) << "unsupported state: " << state;
        return MouseButton::kReleased;
    }
  }

  void Mouse::SetCursorMode(const CursorMode rhs) {
    switch(rhs) {
      case CursorMode::kHidden:
        glfwSetInputMode(GetAppWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        break;
      case CursorMode::kNormal:
        glfwSetInputMode(GetAppWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
      case CursorMode::kCaptured:
        glfwSetInputMode(GetAppWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        break;
      case CursorMode::kDisabled:
        glfwSetInputMode(GetAppWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
      case CursorMode::kUnavailable:
      default:
        DLOG(WARNING) << "cannot set CursorMode to: " << rhs;
        break;
    }
  }

  CursorMode Mouse::GetCursorMode() const {
    const auto mode = glfwGetInputMode(GetAppWindowHandle(), GLFW_CURSOR);
    switch(mode) {
      case GLFW_CURSOR_HIDDEN:
        return CursorMode::kHidden;
      case GLFW_CURSOR_NORMAL:
        return CursorMode::kNormal;
      case GLFW_CURSOR_DISABLED:
        return CursorMode::kDisabled;
      case GLFW_CURSOR_CAPTURED:
        return CursorMode::kCaptured;
      case GLFW_CURSOR_UNAVAILABLE:
      default:
        DLOG(WARNING) << "glfwGetInputMode returned: " << mode;
        return CursorMode::kUnavailable;
    }
  }

  void Mouse::SetCursorPos(const Point& pos) {
    glfwSetCursorPos(GetAppWindowHandle(), pos[0], pos[1]);
  }

  void Mouse::GetCursorPos(Point& pos) const {
    double x, y;
    glfwGetCursorPos(GetAppWindowHandle(), &x, &y);
    pos = Point(floor(x), floor(y));
  }
}

#endif //PRT_GLFW