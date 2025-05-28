#ifndef PRT_WINDOW_H
#define PRT_WINDOW_H

#include <functional>
#include <gflags/gflags.h>
#include <ostream>
#include <set>
#include <string>
#include <yoga/YGConfig.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/dimension.h"
#include "prette/event.h"
#include "prette/geometry/rectangle.h"
#include "prette/geometry/shape.h"
#include "prette/gfx.h"
#include "prette/glm.h"
#include "prette/monitor/monitor.h"
#include "prette/window/window_event.h"

#ifdef PRT_ENABLE_LUA
struct lua_State;
#endif  // PRT_ENABLE_LUA

namespace prt {
static constexpr const auto kDefaultWindowSize = "1280x720";
DECLARE_string(window_size);

static inline auto HasWindowSize() -> bool {
  return !FLAGS_window_size.empty();
}

static inline auto GetWindowSize() -> Dimension {
  return Dimension(FLAGS_window_size);
}

class Window;
class WindowEventSource : public EventSource<WindowEvent> {
  DEFINE_NON_COPYABLE_TYPE(WindowEventSource);

 protected:
  WindowEventSource() = default;

 public:
  ~WindowEventSource() override = default;

#define DEFINE_ON_WINDOW_EVENT(Name)                                     \
  inline auto On##Name##Event()->Name##EventObservable {                 \
    return OnEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
  FOR_EACH_WINDOW_EVENT(DEFINE_ON_WINDOW_EVENT);
#undef DEFINE_ON_WINDOW_EVENT
};

class Mouse;
class Keyboard;
class Window : public WindowEventSource {
  friend class Mouse;
  friend class Keyboard;
  friend class GuiSystem;
  friend class WindowBuilder;
  DEFINE_NON_COPYABLE_TYPE(Window);

 public:
  struct Comparator {
    auto operator()(Window* lhs, Window* rhs) const -> bool {
      ASSERT(lhs);
      ASSERT(rhs);
      return lhs->GetTitle() < rhs->GetTitle();  // TODO: investigate using UUIDs?
    }
  };
#ifdef PRT_GLFW
 public:
  using Handle = GLFWwindow;

 private:
  template <const int Attribute>
  class WindowAttribute {
   public:
    static inline auto Get(Handle* handle) -> int {
      return glfwGetWindowAttrib(handle, Attribute);
    }
  };

  template <const int Attribute>
  class EditableWindowAttribute : public WindowAttribute<Attribute> {
   public:
    static inline void Set(Handle* handle, const int value) {
      return glfwSetWindowAttrib(handle, Attribute, value);
    }

    template <typename T>
    static inline void Set(Handle* handle, const int value) {
      return Set(handle, static_cast<int>(value));
    }
  };

  class DecoratedAttr : public EditableWindowAttribute<GLFW_DECORATED> {};
  class ResizableAttr : public EditableWindowAttribute<GLFW_RESIZABLE> {};
  class FloatingAttr : public EditableWindowAttribute<GLFW_FLOATING> {};
  class AutoIconifyAttr : public EditableWindowAttribute<GLFW_AUTO_ICONIFY> {};
  class FocusOnShowAttr : public EditableWindowAttribute<GLFW_FOCUS_ON_SHOW> {};
  class FocusedAttr : public WindowAttribute<GLFW_FOCUSED> {};
  class IconifiedAttr : public WindowAttribute<GLFW_ICONIFIED> {};
  class MaximizedAttr : public WindowAttribute<GLFW_MAXIMIZED> {};
  class HoveredAttr : public WindowAttribute<GLFW_HOVERED> {};
  class VisibleAttr : public WindowAttribute<GLFW_VISIBLE> {};
  class TransparentFramebufferAttr : public WindowAttribute<GLFW_TRANSPARENT_FRAMEBUFFER> {};

  static inline auto GetWindow(Handle* handle) -> Window* {
    return (Window*)glfwGetWindowUserPointer(handle);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }

  static void OnWindowClosed(Handle* handle);
  static void OnWindowPos(Handle* handle, const int xPos, const int yPos);
  static void OnWindowSize(Handle* handle, const int width, const int height);
  static void OnWindowFocus(Handle* handle, int focused);
  static void OnWindowIconify(Handle* handle, int iconified);
  static void OnWindowRefresh(Handle* handle);
  static void OnWindowMaximize(Handle* handle, int maximized);
  static void OnWindowContentScale(Handle* handle, float xScale, float yScale);
  static void OnWindowFramebufferSize(Handle* handle, const int width, const int height);

  static inline auto Get(Handle* handle) -> Window* {
    ASSERT(handle);
    return ((Window*)glfwGetWindowUserPointer(handle));  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  }
#else
#error "Unsupported Platform."
#endif  // PRT_WINDOW_H
 private:
  uuids::uuid uuid_ = uuids::uuid_system_generator{}();
  WindowEventSubject events_;
  Handle* handle_;
  Keyboard* keyboard_ = nullptr;
  Mouse* mouse_ = nullptr;

  explicit Window(Handle* handle);
  void PublishEvent(WindowEvent* event) const override;
  auto CreateRootYogaNode() const -> YGNodeRef;

  void SetKeyboard(Keyboard* rhs) {
    ASSERT(rhs);
    keyboard_ = rhs;
  }

  void SetMouse(Mouse* rhs) {
    ASSERT(rhs);
    mouse_ = rhs;
  }

#define DEFINE_PUBLISH_EVENT(Name)                       \
  template <typename... Args>                            \
  inline void Publish##Name##Event(Args... args) const { \
    return Publish<Name##Event>(args...);                \
  }
  FOR_EACH_WINDOW_EVENT(DEFINE_PUBLISH_EVENT)
#undef DEFINE_PUBLISH_EVENT

 public:
  ~Window() override;

  auto GetId() const -> const uuids::uuid& {
    return uuid_;
  }

  inline auto GetHandle() const -> Handle* {
    return handle_;
  }

  auto GetTitle() const -> std::string;
  void SetTitle(const std::string& value);
  auto GetPos() const -> Point;
  void SetPos(const Point& value);
  auto GetSize() const -> Dimension;
  void SetSize(const glm::i32vec2& value);
  void Show();
  void Hide();
  void Close();

  void SetDecorated(const bool value) {
    return DecoratedAttr::Set(GetHandle(), value);
  }

  void SetResizable(const bool value) {
    return ResizableAttr::Set(GetHandle(), value);
  }

  void SetFloating(const bool value) {
    return FloatingAttr::Set(GetHandle(), value);
  }

  void SetAutoIconify(const bool value) {
    return AutoIconifyAttr::Set(GetHandle(), value);
  }

  void SetFocusOnShow(const bool value) {
    return FocusOnShowAttr::Set(GetHandle(), value);
  }

  auto IsFocused() const -> bool {
    return FocusedAttr::Get(GetHandle());
  }

  auto IsIconified() const -> bool {
    return IconifiedAttr::Get(GetHandle());
  }

  auto IsMaximized() const -> bool {
    return MaximizedAttr::Get(GetHandle());
  }

  auto IsHovered() const -> bool {
    return HoveredAttr::Get(GetHandle());
  }

  auto IsVisible() const -> bool {
    return VisibleAttr::Get(GetHandle());
  }

  auto IsResizable() const -> bool {
    return ResizableAttr::Get(GetHandle());
  }

  auto IsDecorated() const -> bool {
    return DecoratedAttr::Get(GetHandle());
  }

  auto IsAutoIconify() const -> bool {
    return AutoIconifyAttr::Get(GetHandle());
  }

  auto IsFloating() const -> bool {
    return FloatingAttr::Get(GetHandle());
  }

  auto IsTransparentFramebuffer() const -> bool {
    return TransparentFramebufferAttr::Get(GetHandle());
  }

  auto IsFocusOnShow() const -> bool {
    return FocusOnShowAttr::Get(GetHandle());
  }

  void SwapBuffers();
  auto ToString() const -> std::string;
  void SetMinSize(const glm::i32vec2& size);
  auto GetContentScale() const -> glm::vec2;
  auto ShouldClose() const -> bool;
  auto GetFramebufferSize() const -> Dimension;

  auto GetBounds() const -> Rectangle {
    const auto size = GetSize();
    return Rectangle(Point(), size.width(), size.height());
  }

  auto OnEvent() const -> WindowEventObservable override {
    return events_.get_observable();
  }

  auto GetMouse() const -> Mouse* {
    return mouse_;
  }

  inline auto HasMouse() const -> bool {
    return GetMouse() != nullptr;
  }

  auto GetKeyboard() const -> Keyboard* {
    return keyboard_;
  }

  inline auto HasKeyboard() const -> bool {
    return GetKeyboard() != nullptr;
  }

  friend auto operator<<(std::ostream& stream, Window* rhs) -> std::ostream& {
    return stream << rhs->ToString();
  }

 private:
#ifdef PRT_ENABLE_LUA
  static void InitLua(lua_State* L);
#endif  // PRT_ENABLE_LUA

  static auto New(Handle* handle) -> Window*;

 public:
  static void Init();
};

class Window;
class WindowBuilder {
  DEFINE_NON_COPYABLE_TYPE(WindowBuilder);

 private:
  std::string title_;
  Dimension size_{};
  Monitor* monitor_{};
  Window* share_{};

 private:
  auto GetShareHandle() const -> GLFWwindow*;
  auto GetMonitorHandle() const -> GLFWmonitor*;

 public:
  explicit WindowBuilder(const char* title = "") :
    title_(title),
    size_() {}
  ~WindowBuilder() = default;

  auto HasMonitor() const -> bool {
    return monitor_ != nullptr;
  }

  auto GetMonitor() const -> Monitor* {
    return monitor_;
  }

  void SetSize(const Dimension& size) {
    size_ = size;
  }

  auto GetSize() const -> const Dimension& {
    return size_;
  }

  auto HasShare() const -> bool {
    return share_ != nullptr;
  }

  auto GetShare() const -> Window* {
    return share_;
  }

  void SetShare(Window* share) {
    share_ = share;
  }

  void SetFloating(const bool value);
  void SetResizable(const bool value);
  void SetVisible(const bool value);
  void SetDecorated(const bool value);
  void SetFocused(const bool value);
  void SetAutoIconify(const bool value);
  void SetMaximized(const bool value);
  void SetCenterCursor(const bool value);
  void SetTransparentFramebuffer(const bool value);
  void SetFocusOnShow(const bool value);
  void SetScaleToMonitor(const bool value);
#ifdef OS_IS_OSX
  void SetRetinaFramebuffer(const bool value);
  void SetGraphicsSwitching(const bool value);
#endif

  auto Build() const -> Window*;
};

using WindowSet = std::set<Window*, Window::Comparator>;

auto GetAppWindow() -> Window*;
auto VisitAllWindows(const std::function<bool(Window*)>& vis) -> bool;
auto OnWindowEvent() -> WindowEventObservable;

#define DEFINE_ON_EVENT(Name)                                                  \
  static inline auto On##Name##Event()->Name##EventObservable {                \
    return OnWindowEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_WINDOW_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
}  // namespace prt

#endif  // PRT_WINDOW_H