#ifndef PRT_MOUSE_EVENT_H
#define PRT_MOUSE_EVENT_H

#include <functional>
#include <ostream>
#include <string>
#include <utility>

#include "prette/common.h"
#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/glm.h"
#include "prette/mouse_flags.h"

namespace prt {
#define FOR_EACH_MOUSE_EVENT(V) \
  V(MouseCreated)               \
  V(MouseButtonState)           \
  V(MouseMotion)                \
  V(DragStart)                  \
  V(DragFinished)               \
  V(Scroll)                     \
  V(MouseDestroyed)

class Mouse;
class DragEvent;
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

  inline auto IsButton1() const -> bool {
    return IsButton(GLFW_MOUSE_BUTTON_1);
  }

  inline auto IsButton2() const -> bool {
    return IsButton(GLFW_MOUSE_BUTTON_2);
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

  virtual auto AsDragEvent() -> DragEvent* {
    return nullptr;
  }

  inline auto IsDragEvent() -> bool {
    return AsDragEvent() != nullptr;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(Mouse, FOR_EACH_MOUSE_EVENT);
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
  static inline auto FilterByState(const std::function<bool(const MouseButtonState&)>& filter)
      -> MouseEvent::Predicate {
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

  static inline auto New(const Mouse* mouse, const MouseButtonState& state) -> MouseButtonStateEvent* {
    return new MouseButtonStateEvent(mouse, state);
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

  inline auto IsDown() const -> bool {
    const auto delta = GetDirection();
    return IsMouseScrollInverted() ? (delta.y > 0.0f) : (delta.y < 0.0f);
  }

  inline auto IsUp() const -> bool {
    const auto delta = GetDirection();
    return IsMouseScrollInverted() ? (delta.y < 0.0f) : (delta.y > 0.0f);
  }

  inline auto IsLeft() const -> bool {
    const auto delta = GetDirection();
    return (delta.x < 0.0f);
  }

  inline auto IsRight() const -> bool {
    const auto delta = GetDirection();
    return (delta.x > 0.0f);
  }

  DECLARE_EVENT_TYPE(MouseEvent, MouseMotion);
};

using ScrollDelta = glm::vec2;

class ScrollEvent : public MouseEvent {
 private:
  ScrollDelta delta_;

 public:
  explicit ScrollEvent(const ScrollDelta delta) :
    MouseEvent(),
    delta_(std::move(delta)) {}
  ~ScrollEvent() override = default;

  auto GetDelta() const -> const ScrollDelta& {
    return delta_;
  }

  inline auto GetDeltaY() const -> float {
    return GetDelta().y;
  }

  inline auto GetDeltaX() const -> float {
    return GetDelta().x;
  }

  auto IsScrollUp() const -> bool;
  auto IsScrollDown() const -> bool;
  DECLARE_EVENT_TYPE(MouseEvent, Scroll);
};

struct DragState {
  int button;
  int mods;
  glm::vec2 start;
  glm::vec2 finish;

  void reset() {
    button = 0;
    mods = 0;
    start = glm::vec2();
    finish = glm::vec2();
  }
};

class DragEvent : public MouseEvent {
 protected:
  DragEvent() = default;

 public:
  ~DragEvent() override = default;

  auto AsDragEvent() -> DragEvent* override {
    return this;
  }

 public:
  static inline auto Filter(MouseEvent* rhs) -> bool {
    return rhs && rhs->IsDragEvent();
  }

  static inline auto Cast(MouseEvent* rhs) -> DragEvent* {
    ASSERT(Filter(rhs));
    return rhs->AsDragEvent();
  }
};

class DragStartEvent : public DragEvent {
 private:
  DragState state_{};

 public:
  DragStartEvent(const DragState state) :
    DragEvent(),
    state_(std::move(state)) {}
  ~DragStartEvent() override = default;

  auto GetButton() const -> int {
    return state_.button;
  }

  auto GetPos() const -> const glm::vec2& {
    return state_.start;
  }

  auto GetMods() const -> int {
    return state_.mods;
  }

  DECLARE_EVENT_TYPE(MouseEvent, DragStart);
};

class DragFinishedEvent : public DragEvent {
 private:
  DragState state_;

 public:
  explicit DragFinishedEvent(const DragState state) :
    DragEvent(),
    state_(std::move(state)) {}
  ~DragFinishedEvent() override = default;

  auto GetButton() const -> int {
    return state_.button;
  }

  auto GetStartPos() const -> const glm::vec2& {
    return state_.start;
  }

  auto GetFinishPos() const -> const glm::vec2& {
    return state_.finish;
  }

  inline auto GetDelta() const -> glm::vec2 {
    return GetFinishPos() - GetStartPos();
  }

  auto GetMods() const -> int {
    return state_.mods;
  }

  inline auto IsDragDown() const -> bool {
    const auto delta = GetDelta();
    return IsMouseScrollInverted() ? (delta.y > 0.0f) : (delta.y < 0.0f);
  }

  inline auto IsDragUp() const -> bool {
    const auto delta = GetDelta();
    return IsMouseScrollInverted() ? (delta.y < 0.0f) : (delta.y > 0.0f);
  }

  DECLARE_EVENT_TYPE(MouseEvent, DragFinished);
};

class MouseDestroyedEvent : public MouseEvent {
 public:
  MouseDestroyedEvent() = default;
  ~MouseDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(MouseEvent, MouseDestroyed);
};

DEFINE_EVENT_SUBJECT(Mouse);
DEFINE_EVENT_OBSERVABLE(Drag);
DEFINE_EVENT_OBSERVABLE(Mouse);
FOR_EACH_MOUSE_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_MOUSE_EVENT_H
