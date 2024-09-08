#include "prette/window/window_builder.h"

namespace prt::window {
  auto WindowBuilder::GetShareHandle() const -> GLFWwindow* {
#ifdef PRT_GLFW
    return HasShare() ? GetShare()->GetHandle() : nullptr;
#else
#error "Unsupported Platform"
#endif
  }

  auto WindowBuilder::GetMonitorHandle() const -> GLFWmonitor* {
#ifdef PRT_GLFW
    return HasMonitor() ? GetMonitor()->GetHandle() : nullptr;
#else
#error "Unsupported Platform"
#endif
  }

  static inline void
  SetHint(const int hint, const int value) {
    return glfwWindowHint(hint, value);
  }

  static inline void
  SetHint(const int hint, const bool value) {
    return SetHint(hint, (int)(value ? GLFW_TRUE : GLFW_FALSE));
  }

  void WindowBuilder::SetFloating(const bool value) {
    return SetHint(GLFW_FLOATING, value);
  }

  void WindowBuilder::SetResizable(const bool value) {
    return SetHint(GLFW_RESIZABLE, value);
  }

  void WindowBuilder::SetVisible(const bool value) {
    return SetHint(GLFW_VISIBLE, value);
  }

  void WindowBuilder::SetDecorated(const bool value) {
    return SetHint(GLFW_DECORATED, value);
  }

  void WindowBuilder::SetFocused(const bool value) {
    return SetHint(GLFW_FOCUSED, value);
  }

  void WindowBuilder::SetAutoIconify(const bool value) {
    return SetHint(GLFW_AUTO_ICONIFY, value);
  }

  void WindowBuilder::SetMaximized(const bool value) {
    return SetHint(GLFW_MAXIMIZED, value);
  }

  void WindowBuilder::SetCenterCursor(const bool value) {
    return SetHint(GLFW_CENTER_CURSOR, value);
  }

  void WindowBuilder::SetTransparentFramebuffer(const bool value) {
    return SetHint(GLFW_TRANSPARENT_FRAMEBUFFER, value);
  }

  void WindowBuilder::SetFocusOnShow(const bool value) {
    return SetHint(GLFW_FOCUS_ON_SHOW, value);
  }

  void WindowBuilder::SetScaleToMonitor(const bool value) {
    return SetHint(GLFW_SCALE_TO_MONITOR, value);
  }

#ifdef __APPLE__
  void WindowBuilder::SetRetinaFramebuffer(const bool value) {
    return SetHint(GLFW_COCOA_RETINA_FRAMEBUFFER, value);
  }

  void WindowBuilder::SetGraphicsSwitching(const bool value) {
    return SetHint(GLFW_COCOA_GRAPHICS_SWITCHING, value);
  }
#endif

  Window* WindowBuilder::Build() const {
#ifdef PRT_GLFW
#ifdef __APPLE__
    glfwWindowHintString(GLFW_COCOA_FRAME_NAME, title_.data());
#endif

    const auto handle = glfwCreateWindow(size_[0], size_[1], title_.data(), GetMonitorHandle(), GetShareHandle());
    if(!handle) {
      glfwTerminate();
      LOG(FATAL) << "failed to create Window handle.";
      return nullptr;
    }

    glfwSetWindowCloseCallback(handle, &Window::OnWindowClosed);
    glfwSetWindowPosCallback(handle, &Window::OnWindowPos);
    glfwSetWindowSizeCallback(handle, &Window::OnWindowSize);
    glfwSetWindowFocusCallback(handle, &Window::OnWindowFocus);
    glfwSetWindowIconifyCallback(handle, &Window::OnWindowIconify);
    glfwSetWindowRefreshCallback(handle, &Window::OnWindowRefresh);
    glfwSetWindowMaximizeCallback(handle, &Window::OnWindowMaximize);
    glfwSetWindowContentScaleCallback(handle, &Window::OnWindowContentScale);
    glfwMakeContextCurrent(handle);
    glfwSwapInterval(0);
    return new Window(handle);
#else
#error "Unsupported Platform"
#endif
  }
}