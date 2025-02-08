#ifndef PRT_WINDOW_H
#define PRT_WINDOW_H

#include "prette/builder.h"
#include "prette/dimension.h"
#include "prette/event.h"
#include "prette/geometry/shape.h"
#include "prette/gfx.h"
#include "prette/monitor.h"
#include "prette/uuid.h"

namespace prt {
static constexpr const auto kDefaultWindowSize = "512x512";
DECLARE_string(window_size);

static inline auto HasWindowSize() -> bool {
  return !FLAGS_window_size.empty();
}

static inline auto GetWindowSize() -> Dimension {
  return Dimension(FLAGS_window_size);
}

class Window;

#define FOR_EACH_WINDOW_EVENT(V) \
  V(WindowCreated)               \
  V(WindowOpened)                \
  V(WindowClosed)                \
  V(WindowPos)                   \
  V(WindowSize)                  \
  V(WindowFocus)                 \
  V(WindowIconify)               \
  V(WindowRefresh)               \
  V(WindowMaximize)              \
  V(WindowContentScale)

class Window;
class WindowEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_WINDOW_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class WindowFocusEvent;
class WindowEvent : public Event {
  DEFINE_NON_COPYABLE_TYPE(WindowEvent);

 private:
  const Window* window_;

 public:
  WindowEvent() = delete;
  explicit WindowEvent(const Window* window) :
    window_(window) {
    ASSERT(window_);
  }
  ~WindowEvent() override = default;

  auto GetWindow() const -> const Window* {
    return window_;
  }

  DEFINE_EVENT_PROTOTYPE(FOR_EACH_WINDOW_EVENT);
};

#define DECLARE_WINDOW_EVENT(Name) DECLARE_EVENT_TYPE(WindowEvent, Name)

class WindowCreatedEvent : public WindowEvent {
 public:
  explicit WindowCreatedEvent(const Window* window) :
    WindowEvent(window) {}
  ~WindowCreatedEvent() override = default;
  DECLARE_WINDOW_EVENT(WindowCreated);
};

class WindowOpenedEvent : public WindowEvent {
 public:
  explicit WindowOpenedEvent(const Window* window) :
    WindowEvent(window) {}
  ~WindowOpenedEvent() override = default;
  DECLARE_WINDOW_EVENT(WindowOpened);
};

class WindowClosedEvent : public WindowEvent {
 public:
  explicit WindowClosedEvent(const Window* window) :
    WindowEvent(window) {}
  ~WindowClosedEvent() override = default;
  DECLARE_WINDOW_EVENT(WindowClosed);
};

class WindowPosEvent : public WindowEvent {
 private:
  Point pos_;

 public:
  WindowPosEvent(const Window* window, const Point& pos) :
    WindowEvent(window),
    pos_(pos) {}
  WindowPosEvent(const Window* window, const int32_t x, const int32_t y) :
    WindowPosEvent(window, {x, y}) {}
  ~WindowPosEvent() override = default;

  auto GetPos() const -> const Point& {
    return pos_;
  }

  DECLARE_WINDOW_EVENT(WindowPos);
};

class WindowSizeEvent : public WindowEvent {
 private:
  Dimension size_;

 public:
  WindowSizeEvent(const Window* window, const Dimension& size) :
    WindowEvent(window),
    size_(size) {}
  WindowSizeEvent(const Window* window, const int32_t width, const int32_t height) :
    WindowSizeEvent(window, {width, height}) {}
  explicit WindowSizeEvent(const Window* window) :
    WindowSizeEvent(window, 0, 0) {}
  ~WindowSizeEvent() override = default;

  auto GetSize() const -> const Dimension& {
    return size_;
  }

  DECLARE_WINDOW_EVENT(WindowSize);
};

class WindowFocusEvent : public WindowEvent {
 private:
  bool focused_;

 public:
  WindowFocusEvent(const Window* window, const bool focused) :
    WindowEvent(window),
    focused_(focused) {}
  ~WindowFocusEvent() override = default;

  auto IsFocused() const -> bool {
    return focused_;
  }

  DECLARE_WINDOW_EVENT(WindowFocus);
};

class WindowIconifyEvent : public WindowEvent {
 private:
  bool iconified_;

 public:
  WindowIconifyEvent(const Window* window, const bool iconified) :
    WindowEvent(window),
    iconified_(iconified) {}
  ~WindowIconifyEvent() override = default;

  auto IsIconified() const -> bool {
    return iconified_;
  }

  DECLARE_WINDOW_EVENT(WindowIconify);
};

class WindowRefreshEvent : public WindowEvent {
 public:
  explicit WindowRefreshEvent(const Window* window) :
    WindowEvent(window) {}
  ~WindowRefreshEvent() override = default;
  DECLARE_WINDOW_EVENT(WindowRefresh);
};

class WindowMaximizeEvent : public WindowEvent {
 private:
  bool maximized_;

 public:
  WindowMaximizeEvent(const Window* window, const bool maximized) :
    WindowEvent(window),
    maximized_(maximized) {}
  ~WindowMaximizeEvent() override = default;

  auto IsMaximized() const -> bool {
    return maximized_;
  }

  DECLARE_WINDOW_EVENT(WindowMaximize);
};

class WindowContentScaleEvent : public WindowEvent {
  using Scale = glm::vec2;

 private:
  Scale scale_;

 public:
  WindowContentScaleEvent(const Window* window, const Scale& scale) :
    WindowEvent(window),
    scale_(scale) {}
  WindowContentScaleEvent(const Window* window, const float xScale, const float yScale) :
    WindowContentScaleEvent(window, {xScale, yScale}) {}
  ~WindowContentScaleEvent() override = default;

  auto GetScale() const -> const Scale& {
    return scale_;
  }

  DECLARE_WINDOW_EVENT(WindowContentScale);
};

DEFINE_EVENT_SUBJECT(Window);
DEFINE_EVENT_OBSERVABLE(Window);
FOR_EACH_WINDOW_EVENT(DEFINE_EVENT_OBSERVABLE)

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

class Window : public WindowEventSource {
  friend class Runtime;
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
#else
#error "Unsupported Platform."
#endif  // PRT_WINDOW_H
 private:
  UUID id_;
  WindowEventSubject events_;
  Handle* handle_;

  explicit Window(Handle* handle);
  void PublishEvent(WindowEvent* event) const override;

#define DEFINE_PUBLISH_EVENT(Name)                       \
  template <typename... Args>                            \
  inline void Publish##Name##Event(Args... args) const { \
    return Publish<Name##Event>(this, args...);          \
  }
  FOR_EACH_WINDOW_EVENT(DEFINE_PUBLISH_EVENT)
#undef DEFINE_PUBLISH_EVENT

 public:
  ~Window() override;

  inline auto GetHandle() const -> Handle* {
    return handle_;
  }

  auto GetTitle() const -> std::string;
  void SetTitle(const std::string& value);
  auto GetPos() const -> Point;
  void SetPos(const Point& value);
  auto GetSize() const -> glm::i32vec2;
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
    return Rectangle(Point(), size[0], size[1]);
  }

  auto OnEvent() const -> WindowEventObservable override {
    return events_.get_observable();
  }

  friend auto operator<<(std::ostream& stream, Window* rhs) -> std::ostream& {
    return stream << rhs->ToString();
  }

 private:
  static auto New(Handle* handle) -> Window*;
};

class Window;
class WindowBuilder : public BuilderTemplate<Window> {
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
  WindowBuilder(const char* title = "") :
    BuilderTemplate<Window>(),
    title_(title),
    size_() {}
  ~WindowBuilder() override = default;

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
#ifdef __APPLE__
  void SetRetinaFramebuffer(const bool value);
  void SetGraphicsSwitching(const bool value);
#endif

  auto Build() const -> Window* override;
};

using WindowSet = std::set<Window*, Window::Comparator>;

void InitWindows();
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