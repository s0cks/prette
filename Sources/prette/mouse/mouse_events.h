#ifndef PRT_MOUSE_EVENT_H
#define PRT_MOUSE_EVENT_H

#include <iostream>

#include "prette/rx.h"
#include "prette/event.h"

#include "prette/mouse/mouse_button.h"

namespace prt::mouse {
#define FOR_EACH_MOUSE_EVENT(V) \
  V(MouseCreated)               \
  V(MouseDestroyed)             \
  V(MouseMove)                  \
  V(MouseButtonPressed)         \
  V(MouseButtonReleased)

  class MouseEvent;
  class MouseButtonEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_MOUSE_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  DEFINE_EVENT_SUBJECT(Mouse);
  DEFINE_EVENT_OBSERVABLE(Mouse);
  FOR_EACH_MOUSE_EVENT(DEFINE_EVENT_OBSERVABLE);

  class Mouse;
  class MouseEvent : public Event {
  protected:
    Mouse* mouse_;

    explicit MouseEvent(Mouse* mouse):
      Event(),
      mouse_(mouse) {
    }
  public:
    ~MouseEvent() override = default;

    Mouse* GetMouse() const {
      return mouse_;
    }

    virtual MouseButtonEvent* AsMouseButtonEvent() {
      return nullptr;
    }

    bool IsMouseButtonEvent() {
      return AsMouseButtonEvent() != nullptr;
    }
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_MOUSE_EVENT);
  };

#define DECLARE_MOUSE_EVENT(Name)         \
  DECLARE_EVENT_TYPE(MouseEvent, Name)

  class MouseCreatedEvent : public MouseEvent {
  public:
    explicit MouseCreatedEvent(Mouse* mouse):
      MouseEvent(mouse) {
    }
    ~MouseCreatedEvent() override = default;
    DECLARE_MOUSE_EVENT(MouseCreated);
  };

  class MouseDestroyedEvent : public MouseEvent {
  public:
    explicit MouseDestroyedEvent(Mouse* mouse):
      MouseEvent(mouse) {
    }
    ~MouseDestroyedEvent() override = default;
    DECLARE_MOUSE_EVENT(MouseDestroyed);
  };

  class MouseButtonEvent : public MouseEvent {
  protected:
    glm::vec2 pos_;
    MouseButton button_;
    MouseButton::State state_;
  public:
    MouseButtonEvent(Mouse* mouse,
                     const glm::vec2& pos,
                     const MouseButton button,
                     const MouseButton::State state):
      MouseEvent(mouse),
      pos_(pos),
      button_(button),
      state_(state) {
    }
    ~MouseButtonEvent() override = default;

    const glm::vec2& GetPos() const {
      return pos_;
    }

    MouseButton GetButton() const {
      return button_;
    }

    MouseButton::State GetState() const {
      return state_;
    }

#define DEFINE_STATE_CHECK(Name, Value)                                     \
    bool Is##Name() const { return GetState() == MouseButton::k##Name; }
    FOR_EACH_MOUSE_BUTTON_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK
  };

  class MouseButtonPressedEvent : public MouseButtonEvent {
  public:
    MouseButtonPressedEvent(Mouse* mouse, const glm::vec2& pos, const MouseButton button):
      MouseButtonEvent(mouse, pos, button, MouseButton::kPressed) {
    }
    ~MouseButtonPressedEvent() override = default;
    DECLARE_MOUSE_EVENT(MouseButtonPressed);
  };

  class MouseButtonReleasedEvent : public MouseButtonEvent {
  public:
    MouseButtonReleasedEvent(Mouse* mouse, const glm::vec2& pos, const MouseButton button):
      MouseButtonEvent(mouse, pos, button, MouseButton::kReleased) {
    }
    ~MouseButtonReleasedEvent() override = default;
    DECLARE_MOUSE_EVENT(MouseButtonReleased);
  };

  class MouseMoveEvent : public MouseEvent {
  protected:
    glm::vec2 pos_;
    glm::vec2 last_pos_;
    glm::vec2 delta_;
  public:
    explicit MouseMoveEvent(Mouse* mouse,
                            const glm::vec2 pos,
                            const glm::vec2 last_pos):
      MouseEvent(mouse),
      pos_(pos),
      last_pos_(last_pos),
      delta_(pos - last_pos) {
    }
    ~MouseMoveEvent() override = default;

    const glm::vec2& pos() const {
      return pos_;
    }

    const glm::vec2& last_pos() const {
      return last_pos_;
    }

    const glm::vec2& delta() const {
      return delta_;
    }

    DECLARE_MOUSE_EVENT(MouseMove);
  };

  DEFINE_EVENT_SUBJECT(Mouse);
  DEFINE_EVENT_OBSERVABLE(Mouse);
  FOR_EACH_MOUSE_EVENT(DEFINE_EVENT_OBSERVABLE);

  class MouseEventSource : public EventSource<MouseEvent> {
  protected:
    MouseEventSource() = default;
  public:
    ~MouseEventSource() override = default;

#define DEFINE_ON_EVENT(Name)             \
    inline Name##EventObservable          \
    On##Name##Event() const {             \
      return OnEvent()                    \
        .filter(Name##Event::Filter)      \
        .map(Name##Event::Cast);          \
    }
    FOR_EACH_MOUSE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };

#define DECLARE_EVENT_SUBSCRIBER(Name)                                  \
  class Name##EventSubscriber {                                         \
  private:                                                              \
    rx::subscription sub_;                                              \
  protected:                                                            \
    explicit Name##EventSubscriber(Mouse* mouse);                       \
    virtual void On##Name(Name##Event* event) = 0;                      \
  public:                                                               \
    virtual ~Name##EventSubscriber() {                                  \
      if(sub_.is_subscribed())                                          \
        sub_.unsubscribe();                                             \
    }                                                                   \
  };
  FOR_EACH_MOUSE_EVENT(DECLARE_EVENT_SUBSCRIBER)
#undef DECLARE_EVENT_SUBSCRIBER
}

#endif //PRT_MOUSE_EVENT_H