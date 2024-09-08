#ifndef PRT_WINDOW_EVENTS_H
#define PRT_WINDOW_EVENTS_H

#include "prette/event.h"
#include "prette/shape.h"
#include "prette/dimension.h"

namespace prt::window {
#define FOR_EACH_WINDOW_EVENT(V)   \
  V(WindowOpened)                  \
  V(WindowClosed)                  \
  V(WindowPos)                     \
  V(WindowSize)                    \
  V(WindowFocus)                   \
  V(WindowIconify)                 \
  V(WindowRefresh)                 \
  V(WindowMaximize)                \
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
    Window* window_;
  public:
    WindowEvent() = delete;
    explicit WindowEvent(Window* window):
      window_(window) {
    }
    ~WindowEvent() override = default;

    auto GetWindow() const -> Window* {
      return window_;
    }

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_WINDOW_EVENT);
  };

#define DECLARE_WINDOW_EVENT(Name)        \
  DECLARE_EVENT_TYPE(WindowEvent, Name)

  class WindowOpenedEvent : public WindowEvent {
  public:
    explicit WindowOpenedEvent(Window* window):
      WindowEvent(window) {
    }
    ~WindowOpenedEvent() override = default;
    DECLARE_WINDOW_EVENT(WindowOpened);
  };

  class WindowClosedEvent : public WindowEvent {
  public:
    explicit WindowClosedEvent(Window* window):
      WindowEvent(window) {
    }
    ~WindowClosedEvent() override = default;
    DECLARE_WINDOW_EVENT(WindowClosed);
  };

  class WindowPosEvent : public WindowEvent {
  private:
    Point pos_;
  public:
    WindowPosEvent(Window* window, const Point& pos):
      WindowEvent(window),
      pos_(pos) {
    }
    explicit WindowPosEvent(Window* window, const int32_t x, const int32_t y):
      WindowPosEvent(window, { x, y }) {
    }
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
    WindowSizeEvent(Window* window, const Dimension& size):
      WindowEvent(window),
      size_(size) {
    }
    WindowSizeEvent(Window* window, const int32_t width, const int32_t height):
      WindowSizeEvent(window, { width, height }) {
    }
    explicit WindowSizeEvent(Window* window):
      WindowSizeEvent(window, 0, 0) {
    }
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
    WindowFocusEvent(Window* window, const bool focused):
      WindowEvent(window),
      focused_(focused) {
    }
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
    WindowIconifyEvent(Window* window, bool iconified):
      WindowEvent(window),
      iconified_(iconified) {
    }
    ~WindowIconifyEvent() override = default;

    auto IsIconified() const -> bool {
      return iconified_;
    }

    DECLARE_WINDOW_EVENT(WindowIconify);
  };

  class WindowRefreshEvent : public WindowEvent {
  public:
    WindowRefreshEvent(Window* window):
      WindowEvent(window) {
    }
    ~WindowRefreshEvent() override = default;
    DECLARE_WINDOW_EVENT(WindowRefresh);
  };

  class WindowMaximizeEvent : public WindowEvent {
  private:
    bool maximized_;
  public:
    WindowMaximizeEvent(Window* window, const bool maximized):
      WindowEvent(window),
      maximized_(maximized) {
    }
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
    WindowContentScaleEvent(Window* window, const Scale& scale):
      WindowEvent(window),
      scale_(scale) {
    }
    WindowContentScaleEvent(Window* window, const float xScale, const float yScale):
      WindowContentScaleEvent(window, { xScale, yScale }) {
    }
    ~WindowContentScaleEvent() override = default;

    auto GetScale() const -> const Scale& {
      return scale_;
    }

    DECLARE_WINDOW_EVENT(WindowContentScale);
  };

  using WindowEventSubject = rx::subject<WindowEvent*>;
  using WindowEventObservable = rx::observable<WindowEvent*>;
#define DEFINE_EVENT_OBSERVABLE(Name) \
  using Name##EventObservable = rx::observable<Name##Event*>;
  FOR_EACH_WINDOW_EVENT(DEFINE_EVENT_OBSERVABLE)
#undef DEFINE_EVENT_OBSERVABLE

  class WindowEventSource : public EventSource<WindowEvent> {
    DEFINE_NON_COPYABLE_TYPE(WindowEventSource);
  protected:
    WindowEventSource() = default;
  public:
    ~WindowEventSource() override = default;

#define DEFINE_ON_WINDOW_EVENT(Name)              \
    inline auto                                   \
    On##Name##Event() -> Name##EventObservable {  \
      return OnEvent()                            \
        .filter(Name##Event::Filter)              \
        .map(Name##Event::Cast);                  \
    }
    FOR_EACH_WINDOW_EVENT(DEFINE_ON_WINDOW_EVENT);
#undef DEFINE_ON_WINDOW_EVENT
  };
}

#endif //PRT_WINDOW_EVENTS_H