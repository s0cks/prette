#ifndef PRT_MOUSE_H
#define PRT_MOUSE_H

#include "prette/common.h"
#include "prette/event.h"
#include "prette/gfx.h"

namespace prt {
#define FOR_EACH_MOUSE_EVENT(V) \
  V(MouseCreated)               \
  V(MouseButtonState)           \
  V(MouseMotion)                \
  V(MouseDestroyed)

class Mouse;
class MouseEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_MOUSE_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class MouseButtonState {
  DEFINE_DEFAULT_COPYABLE_TYPE(MouseButtonState);

 private:
  int button_;
  int action_;
  int mods_;

 public:
  MouseButtonState() = default;
  MouseButtonState(const int button, const int action, const int mods = 0) :
    button_(button),
    action_(action),
    mods_(mods) {}
  ~MouseButtonState() = default;

  auto GetButton() const -> int {
    return button_;
  }

  auto GetAction() const -> int {
    return action_;
  }

  auto GetMods() const -> int {
    return mods_;
  }

  inline auto IsAction(const int action) const -> bool {
    return GetAction() == action;
  }

  inline auto IsPressed() const -> bool {
    return IsAction(GLFW_PRESS);
  }

  inline auto IsReleased() const -> bool {
    return IsAction(GLFW_RELEASE);
  }

  inline auto IsButton(const int button) const -> bool {
    return GetButton() == button;
  }

  auto ToString() const -> std::string;

  friend auto operator<<(std::ostream& stream, const MouseButtonState& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }

  auto operator==(const MouseButtonState& rhs) const -> bool {
    return button_ == rhs.button_ && action_ == rhs.action_;
  }

  auto operator!=(const MouseButtonState& rhs) const -> bool {
    return button_ != rhs.button_ || action_ != rhs.action_;
  }
};

class MouseEvent : public Event {
 protected:
  MouseEvent() = default;

 public:
  ~MouseEvent() override = default;
  DEFINE_EVENT_PROTOTYPE(Mouse, FOR_EACH_MOUSE_EVENT);
};

class MouseEventBase : public MouseEvent {
 private:
  const Mouse* mouse_;

 protected:
  explicit MouseEventBase(const Mouse* mouse) :
    MouseEvent(),
    mouse_(mouse) {
    ASSERT(mouse_);
  }

 public:
  ~MouseEventBase() override = default;

  auto GetMouse() const -> const Mouse* {
    return mouse_;
  }
};

class MouseCreatedEvent : public MouseEventBase {
 public:
  explicit MouseCreatedEvent(const Mouse* mouse) :
    MouseEventBase(mouse) {}
  ~MouseCreatedEvent() override = default;
  DECLARE_EVENT_TYPE(MouseEvent, MouseCreated);
};

class MouseButtonStateEvent : public MouseEventBase {
 private:
  MouseButtonState state_;

 public:
  MouseButtonStateEvent(const Mouse* mouse, const MouseButtonState& state) :
    MouseEventBase(mouse),
    state_(state) {}
  ~MouseButtonStateEvent() override = default;

  auto GetState() const -> const MouseButtonState& {
    return state_;
  }

  DECLARE_EVENT_TYPE(MouseEvent, MouseButtonState);

 public:
  static inline auto FilterByState(const std::function<bool(const MouseButtonState&)>& filter) -> MouseEvent::Predicate {
    return [filter](MouseEvent* event) {
      return event && event->IsMouseButtonStateEvent() && filter(event->AsMouseButtonStateEvent()->GetState());
    };
  }

  static inline auto FilterByButton(const int button) -> MouseEvent::Predicate {
    return FilterByState([button](const MouseButtonState& state) {
      return state.IsButton(button);
    });
  }

  static inline auto FilterByState(const MouseButtonState& rhs) -> MouseEvent::Predicate {
    return FilterByState([=](const MouseButtonState& lhs) {
      return lhs == rhs;
    });
  }

  static inline auto FilterByPressed() -> MouseEvent::Predicate {
    return FilterByState([](const MouseButtonState& state) {
      return state.IsPressed();
    });
  }

  static inline auto FilterByPressed(const int button, const int mods = 0) -> MouseEvent::Predicate {
    return FilterByState(MouseButtonState(button, GLFW_PRESS, mods));
  }

  static inline auto FilterByReleased() -> MouseEvent::Predicate {
    return FilterByState([](const MouseButtonState& state) {
      return state.IsReleased();
    });
  }

  static inline auto FilterByReleased(const int button) -> MouseEvent::Predicate {
    return FilterByState([button](const MouseButtonState& state) {
      return state.IsButton(button);
    });
  }
};

class MouseMotionEvent : public MouseEventBase {
 private:
  glm::vec2 direction_;

 public:
  MouseMotionEvent(const Mouse* mouse, const glm::vec2& dir) :
    MouseEventBase(mouse),
    direction_(dir) {}
  MouseMotionEvent(const Mouse* mouse, const double xPos, const double yPos) :
    MouseMotionEvent(mouse, glm::vec2(xPos, yPos)) {}
  ~MouseMotionEvent() = default;

  auto GetDirection() const -> glm::vec2 const& {
    return direction_;
  }

  DECLARE_EVENT_TYPE(MouseEvent, MouseMotion);
};

class MouseDestroyedEvent : public MouseEvent {
 public:
  MouseDestroyedEvent() = default;
  ~MouseDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(MouseEvent, MouseDestroyed);
};

DEFINE_EVENT_SUBJECT(Mouse);
DEFINE_EVENT_OBSERVABLE(Mouse);
FOR_EACH_MOUSE_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnMouseEvent() -> MouseEventObservable;

#define DEFINE_ON_EVENT(Name)                                                 \
  static inline auto On##Name##Event()->Name##EventObservable {               \
    return OnMouseEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_MOUSE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

static inline auto OnMouseButton(const int button) -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByButton(button)).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMousePressed() -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByPressed()).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMousePressed(const int button) -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByPressed(button)).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMouseReleased() -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByReleased()).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMouseReleased(const int button) -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByReleased(button)).map(MouseButtonStateEvent::Cast);
}

class Window;
class Mouse {
  friend class LuaState;
#ifdef PRT_GLFW
  static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void OnMouseMotion(GLFWwindow* window, double xPos, double yPos);
#endif  // PRT_GLFW
 private:
  Window* owner_;
  glm::vec2 previous_pos_{};

  static void PublishEvent(MouseEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

 protected:
  explicit Mouse(Window* owner);

 public:
  ~Mouse();

  auto GetOwner() const -> Window* {
    return owner_;
  }

  auto GetPos() const -> glm::dvec2;

  auto GetPreviousPos() const -> glm::vec2 const& {
    return previous_pos_;
  }

  auto IsPressed(const int btn) const -> bool;

 private:
  static auto New(Window* owner) -> Mouse*;
  static void InitLua(lua_State* L);

 public:
  static auto IsInitialized() -> bool;
  static auto Init(Window* owner) -> Mouse*;
  static auto Get() -> Mouse*;
};

}  // namespace prt

#endif  // PRT_MOUSE_H
