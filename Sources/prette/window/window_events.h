#ifndef PRT_WINDOW_EVENTS_H
#define PRT_WINDOW_EVENTS_H

#include "prette/event.h"
#include "prette/shape.h"
#include "prette/dimension.h"

namespace prt {
  namespace window {
    class Window;
  }
  using window::Window;

  class WindowEvent;

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

#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_WINDOW_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class WindowFocusEvent;
  class WindowEvent : public Event {
    DEFINE_NON_COPYABLE_TYPE(WindowEvent);
  protected:
    Window* window_;
  public:
    WindowEvent() = delete;
    explicit WindowEvent(Window* window):
      window_(window) {
    }
    ~WindowEvent() override = default;

    Window* window() const {
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
  protected:
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

    Point pos() const {
      return pos_;
    }

    inline int32_t x() const {
      return pos_[0];
    }

    inline int32_t y() const {
      return pos_[1];
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
  
    Dimension size() const {
      return size_;
    }

    inline int32_t width() const {
      return size_.width();
    }

    inline int32_t height() const {
      return size_.height();
    }

    DECLARE_WINDOW_EVENT(WindowSize);
  };

  class WindowFocusEvent : public WindowEvent {
  protected:
    bool focused_;
  public:
    WindowFocusEvent(Window* window, const bool focused):
      WindowEvent(window),
      focused_(focused) {
    }
    ~WindowFocusEvent() override = default;

    bool IsFocused() const {
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

    bool iconified() const {
      return iconified_;
    }

    DECLARE_WINDOW_EVENT(WindowIconify);
  };

  class WindowRefreshEvent : public WindowEvent {
  public:
    WindowRefreshEvent(Window* window):
      WindowEvent(window) {
    }
    ~WindowRefreshEvent() = default;
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

    bool maximized() const {
      return maximized_;
    }

    DECLARE_WINDOW_EVENT(WindowMaximize);
  };

  class WindowContentScaleEvent : public WindowEvent {
  private:
    glm::vec2 scale_;
  public:
    WindowContentScaleEvent(Window* window, const glm::vec2 scale):
      WindowEvent(window),
      scale_(scale) {
    }
    WindowContentScaleEvent(Window* window, const float xScale, const float yScale):
      WindowContentScaleEvent(window, glm::vec2(xScale, yScale)) {
    }
    ~WindowContentScaleEvent() override = default;

    glm::vec2 scale() const {
      return scale_;
    }

    DECLARE_WINDOW_EVENT(WindowContentScale);
  };

  typedef rx::subject<WindowEvent*> WindowEventSubject;

  class WindowEventPublisher : public EventPublisher<WindowEvent> {
  protected:
    WindowEventPublisher() = default;
  public:
    ~WindowEventPublisher() override = default;

#define DEFINE_ON_WINDOW_EVENT(Name)      \
    inline rx::observable<Name##Event*>   \
    On##Name##Event() {                   \
      return OnEvent()                    \
        .filter(Name##Event::Filter)      \
        .map(Name##Event::Cast);          \
    }
    FOR_EACH_WINDOW_EVENT(DEFINE_ON_WINDOW_EVENT);
#undef DEFINE_ON_WINDOW_EVENT
  };
}

#endif //PRT_WINDOW_EVENTS_H