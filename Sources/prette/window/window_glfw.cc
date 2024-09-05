
#include "prette/window/window.h"
#ifdef PRT_GLFW

#include "prette/gfx.h"
#include "prette/thread_local.h"

#include "prette/engine/engine.h"
#include "prette/window/monitor.h"
#include "prette/render/renderer.h"

namespace prt::window {
  void Window::OnWindowClosed(Handle* handle) {
    const auto engine = engine::GetEngine();
    PRT_ASSERT(engine);
    engine->Shutdown();
  }

  void Window::OnWindowPos(Handle* handle, const int xPos, const int yPos) {
    const auto window = GetWindow(handle);
    PRT_ASSERT(window);
    return window->Publish<WindowPosEvent>(window, xPos, yPos);
  }

  void Window::OnWindowSize(Handle* handle, const int width, const int height) {
    const auto window = GetWindow(handle);
    PRT_ASSERT(window);
    window->GetFrame()->SetBounds(Rectangle(Point(0, 0), width, height));
    return window->Publish<WindowSizeEvent>(window, width, height);
  }

  void Window::OnWindowFocus(Handle* handle, int focused) {
    const auto window = GetWindow(handle);
    PRT_ASSERT(window);
    return window->Publish<WindowFocusEvent>(window, static_cast<bool>(focused));
  }

  void Window::OnWindowIconify(Handle* handle, int iconified) {
    const auto window = GetWindow(handle);
    PRT_ASSERT(window);
    return window->Publish<WindowIconifyEvent>(window, static_cast<bool>(iconified));
  }

  void Window::OnWindowRefresh(Handle* handle) {
    const auto window = GetWindow(handle);
    PRT_ASSERT(window);
    return window->Publish<WindowRefreshEvent>(window);
  }

  void Window::OnWindowMaximize(Handle* handle, int maximized) {
    const auto window = GetWindow(handle);
    PRT_ASSERT(window);
    return window->Publish<WindowMaximizeEvent>(window, static_cast<bool>(maximized));
  }

  void Window::OnWindowContentScale(Handle* handle, float xScale, float yScale) {
    const auto window = GetWindow(handle);
    PRT_ASSERT(window);
    return window->Publish<WindowContentScaleEvent>(window, xScale, yScale);
  }

  std::string Window::GetTitle() const {
    return std::string(glfwGetWindowTitle(GetHandle()));
  }

  void Window::SetTitle(const std::string& title) {
    glfwSetWindowTitle(GetHandle(), title.data());
  }

  void Window::Hide() {
    glfwHideWindow(GetHandle());
    return Publish<WindowClosedEvent>(this);
  }

  void Window::Show() {
    glfwShowWindow(GetHandle());
    return Publish<WindowOpenedEvent>(this);
  }

  void Window::SetMinSize(const glm::i32vec2& size) {
    glfwSetWindowSizeLimits(GetHandle(), size[0], size[1], GLFW_DONT_CARE, GLFW_DONT_CARE);
  }

  glm::vec2 Window::GetContentScale() const {
    glm::vec2 scale;
    glfwGetWindowContentScale(GetHandle(), &scale[0], &scale[1]);
    return scale;
  }

  glm::i32vec2 Window::GetSize() const {
    glm::i32vec2 size;
    glfwGetWindowSize(GetHandle(), &size[0], &size[1]);
    return size;
  }

  void Window::SetSize(const glm::i32vec2& size) {
    glfwSetWindowSize(GetHandle(), size[0], size[1]);
  }

  void Window::SetPos(const Point& pos) {
    glfwSetWindowPos(GetHandle(), pos[0], pos[1]);
  }

  Point Window::GetPos() const {
    glm::i32vec2 pos;
    glfwGetWindowPos(GetHandle(), &pos[0], &pos[1]);
    return Point(pos);
  }

  void Window::Close() {
    glfwSetWindowShouldClose(GetHandle(), true);
    Publish<WindowClosedEvent>(this);
  }

  void Window::SwapBuffers() {
    glfwSwapBuffers(GetHandle());
    CHECK_GL;
  }
}

#endif //PRT_GLFW