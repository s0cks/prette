#include "prette/window/window.h"
#ifdef PRT_GLFW

#include <string>

#include "prette/common.h"
#include "prette/dimension.h"
#include "prette/engine/engine.h"
#include "prette/exception.h"
#include "prette/geometry/shape.h"
#include "prette/gfx.h"
#include "prette/glm.h"
#include "prette/monitor/monitor.h"
#include "prette/window/window_event.h"

namespace prt {
void Window::OnWindowClosed(Handle* handle) {
  const auto engine = GetEngine();
  ASSERT(engine);
  engine->Shutdown();
}

void Window::OnWindowPos(Handle* handle, const int xPos, const int yPos) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowPosEvent>(xPos, yPos);
}

void Window::OnWindowSize(Handle* handle, const int width, const int height) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowSizeEvent>(width, height);
}

void Window::OnWindowFocus(Handle* handle, int focused) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowFocusEvent>(static_cast<bool>(focused));
}

void Window::OnWindowIconify(Handle* handle, int iconified) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowIconifyEvent>(static_cast<bool>(iconified));
}

void Window::OnWindowRefresh(Handle* handle) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowRefreshEvent>();
}

void Window::OnWindowMaximize(Handle* handle, int maximized) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowMaximizeEvent>(static_cast<bool>(maximized));
}

void Window::OnWindowContentScale(Handle* handle, float xScale, float yScale) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowContentScaleEvent>(xScale, yScale);
}

void Window::OnWindowFramebufferSize(Handle* handle, const int width, const int height) {
  const auto window = GetWindow(handle);
  ASSERT(window);
  return window->Publish<WindowFramebufferSizeEvent>(width, height);
}

auto Window::GetTitle() const -> std::string {
  return {glfwGetWindowTitle(GetHandle())};
}

void Window::SetTitle(const std::string& title) {
  glfwSetWindowTitle(GetHandle(), title.data());
}

void Window::Hide() {
  glfwHideWindow(GetHandle());
  return Publish<WindowClosedEvent>();
}

void Window::Show() {
  glfwShowWindow(GetHandle());
  return Publish<WindowOpenedEvent>();
}

void Window::SetMinSize(const glm::i32vec2& size) {
  glfwSetWindowSizeLimits(GetHandle(), size[0], size[1], GLFW_DONT_CARE, GLFW_DONT_CARE);
}

auto Window::GetContentScale() const -> glm::vec2 {
  glm::vec2 scale;
  glfwGetWindowContentScale(GetHandle(), &scale[0], &scale[1]);
  return scale;
}

auto Window::ShouldClose() const -> bool {
  return glfwWindowShouldClose(GetHandle()) == GLFW_TRUE;
}

auto Window::GetSize() const -> Dimension {
  int width = 0, height = 0;
  glfwGetWindowSize(GetHandle(), &width, &height);
  return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

void Window::SetSize(const glm::i32vec2& size) {
  glfwSetWindowSize(GetHandle(), size[0], size[1]);
}

void Window::SetPos(const Point& pos) {
  glfwSetWindowPos(GetHandle(), static_cast<int>(pos[0]), static_cast<int>(pos[1]));
}

auto Window::GetPos() const -> Point {
  glm::i32vec2 pos;
  glfwGetWindowPos(GetHandle(), &pos[0], &pos[1]);
  return {pos};
}

auto Window::GetFramebufferSize() const -> Dimension {
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(GetHandle(), &width, &height);
  return {width, height};
}

void Window::Close() {
  glfwSetWindowShouldClose(GetHandle(), true);
  Publish<WindowClosedEvent>();
}

void Window::SwapBuffers() {
  glfwSwapBuffers(GetHandle());
}

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

static inline void SetHint(const int hint, const int value) {
  return glfwWindowHint(hint, value);
}

static inline void SetHint(const int hint, const bool value) {
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

auto WindowBuilder::Build() const -> Window* {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#ifdef __APPLE__
  glfwWindowHintString(GLFW_COCOA_FRAME_NAME, title_.data());
#endif
  const auto handle = glfwCreateWindow(static_cast<int>(size_.width()), static_cast<int>(size_.height()), title_.data(),
                                       GetMonitorHandle(), GetShareHandle());
  if (!handle) {
    const auto engine = GetEngine();
    ASSERT(engine);
    engine->Shutdown(Exception::New("failed to create glfw window handle"));
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
  glfwSetFramebufferSizeCallback(handle, &Window::OnWindowFramebufferSize);

#ifdef PRT_GL
  glfwMakeContextCurrent(handle);
  glfwSwapInterval(0);
#endif  // PRT_GL
  return Window::New(handle);
}
}  // namespace prt

#endif  // PRT_GLFW