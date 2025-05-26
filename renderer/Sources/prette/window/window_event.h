#ifndef PRT_WINDOW_EVENT_H
#define PRT_WINDOW_EVENT_H

#include "prette/common.h"
#include "prette/dimension.h"
#include "prette/event.h"
#include "prette/geometry/shape.h"
#include "prette/glm.h"
#include "prette/lua.h"
#include "prette/platform.h"

namespace prt {
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
  V(WindowContentScale)          \
  V(WindowFramebufferSize)

class Window;
class WindowEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_WINDOW_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class WindowEvent : public Event {
  DEFINE_NON_COPYABLE_TYPE(WindowEvent);

 public:
  WindowEvent() = default;
  ~WindowEvent() override = default;

#ifdef PRT_ENABLE_LUA
  void ToTable(lua_State* L) const override;
#endif  // PRT_ENABLE_LUA
  DEFINE_EVENT_PROTOTYPE_TYPE(Window, FOR_EACH_WINDOW_EVENT);
};

#define DECLARE_WINDOW_EVENT_TYPE(Name) DECLARE_EVENT_TYPE(WindowEvent, Name)

#define DEFINE_WINDOW_EVENT(Name)          \
  class Name##Event : public WindowEvent { \
   public:                                 \
    Name##Event() = default;               \
    ~Name##Event() override = default;     \
    DECLARE_WINDOW_EVENT_TYPE(Name);       \
  };

DEFINE_WINDOW_EVENT(WindowCreated);
DEFINE_WINDOW_EVENT(WindowOpened);
DEFINE_WINDOW_EVENT(WindowClosed);

class WindowPosEvent : public WindowEvent {
 private:
  Point pos_;

 public:
  explicit WindowPosEvent(const Point& pos) :
    WindowEvent(),
    pos_(pos) {}
  WindowPosEvent(const int32_t x, const int32_t y) :
    WindowEvent(),
    pos_(x, y) {}
  ~WindowPosEvent() override = default;

  auto GetPos() const -> const Point& {
    return pos_;
  }

  DECLARE_WINDOW_EVENT_TYPE(WindowPos);
};

class WindowSizeEvent : public WindowEvent {
 private:
  Dimension size_;

 public:
  WindowSizeEvent(const Dimension& size) :
    WindowEvent(),
    size_(size) {}
  WindowSizeEvent(const int32_t width, const int32_t height) :
    WindowEvent(),
    size_(width, height) {}
  ~WindowSizeEvent() override = default;

  auto GetSize() const -> const Dimension& {
    return size_;
  }

  DECLARE_WINDOW_EVENT_TYPE(WindowSize);
};

class WindowFocusEvent : public WindowEvent {
 private:
  bool focused_;

 public:
  explicit WindowFocusEvent(const bool focused) :
    WindowEvent(),
    focused_(focused) {}
  ~WindowFocusEvent() override = default;

  auto IsFocused() const -> bool {
    return focused_;
  }

#ifdef PRT_ENABLE_LUA
  void ToTable(lua_State* L) const override;
#endif  // PRT_ENABLE_LUA
  DECLARE_WINDOW_EVENT_TYPE(WindowFocus);
};

class WindowIconifyEvent : public WindowEvent {
 private:
  bool iconified_;

 public:
  explicit WindowIconifyEvent(const bool iconified) :
    WindowEvent(),
    iconified_(iconified) {}
  ~WindowIconifyEvent() override = default;

  auto IsIconified() const -> bool {
    return iconified_;
  }

  DECLARE_WINDOW_EVENT_TYPE(WindowIconify);
};

DEFINE_WINDOW_EVENT(WindowRefresh);

class WindowMaximizeEvent : public WindowEvent {
 private:
  bool maximized_;

 public:
  explicit WindowMaximizeEvent(const bool maximized) :
    WindowEvent(),
    maximized_(maximized) {}
  ~WindowMaximizeEvent() override = default;

  auto IsMaximized() const -> bool {
    return maximized_;
  }

  DECLARE_WINDOW_EVENT_TYPE(WindowMaximize);
};

class WindowFramebufferSizeEvent : public WindowEvent {
 private:
  uint32_t width_;
  uint32_t height_;

 public:
  WindowFramebufferSizeEvent(const uint32_t width, const uint32_t height) :
    WindowEvent(),
    width_(width),
    height_(height) {}
  ~WindowFramebufferSizeEvent() override = default;

  auto GetWidth() const -> uint32_t {
    return width_;
  }

  auto GetHeight() const -> uint32_t {
    return height_;
  }

  DECLARE_WINDOW_EVENT_TYPE(WindowFramebufferSize);
};

class WindowContentScaleEvent : public WindowEvent {
  using Scale = glm::vec2;

 private:
  Scale scale_;

 public:
  explicit WindowContentScaleEvent(const Scale& scale) :
    WindowEvent(),
    scale_(scale) {}
  WindowContentScaleEvent(const float xScale, const float yScale) :
    WindowEvent(),
    scale_(xScale, yScale) {}
  ~WindowContentScaleEvent() override = default;

  auto GetScale() const -> const Scale& {
    return scale_;
  }

  DECLARE_WINDOW_EVENT_TYPE(WindowContentScale);
};

DEFINE_EVENT_SUBJECT(Window);
DEFINE_EVENT_OBSERVABLE(Window);
FOR_EACH_WINDOW_EVENT(DEFINE_EVENT_OBSERVABLE)
}  // namespace prt

#endif  // PRT_WINDOW_EVENT_H
