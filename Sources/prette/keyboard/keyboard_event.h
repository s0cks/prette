#ifndef PRT_KEYBOARD_EVENT_H
#define PRT_KEYBOARD_EVENT_H

#include "prette/event.h"
#include "prette/common.h"
#include "prette/keyboard/key.h"

namespace prt::keyboard {
#define FOR_EACH_KEYBOARD_EVENT(V) \
  V(KeyboardCreated)               \
  V(KeyboardDestroyed)             \
  V(KeyPressed)                    \
  V(KeyReleased)

  class Keyboard;
  class KeyEvent;
  class KeyboardEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_KEYBOARD_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

#define DECLARE_KEYBOARD_EVENT(Name)                            \
  public:                                                       \
    Name##Event* As##Name##Event() override { return this; }    \
    const char* GetName() const override { return #Name; }      \
    std::string ToString() const override;                      \
    static inline bool                                          \
    Filter(KeyboardEvent* event) {                              \
      return event                                              \
          && event->Is##Name##Event();                          \
    }                                                           \
    static inline Name##Event*                                  \
    Cast(KeyboardEvent* event) {                                \
      PRT_ASSERT(event);                                        \
      PRT_ASSERT(event->Is##Name##Event());                     \
      return event->As##Name##Event();                          \
    }

  class KeyboardEvent : public Event {
  protected:
    const Keyboard* keyboard_;

    explicit KeyboardEvent(const Keyboard* keyboard):
      Event(),
      keyboard_(keyboard) {
    }
  public:
    ~KeyboardEvent() override = default;

    const Keyboard* keyboard() const {
      return keyboard_;
    }


    virtual KeyEvent* AsKeyEvent() {
      return nullptr;
    }

    bool IsKeyEvent() {
      return AsKeyEvent() != nullptr;
    }

#define DEFINE_TYPE_CHECK(Name)                                         \
    virtual Name##Event* As##Name##Event() { return nullptr; }          \
    bool Is##Name##Event() { return As##Name##Event() != nullptr; }
    FOR_EACH_KEYBOARD_EVENT(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
  };

  class KeyboardCreatedEvent : public KeyboardEvent {
  public:
    explicit KeyboardCreatedEvent(const Keyboard* keyboard):
      KeyboardEvent(keyboard) {
    }
    ~KeyboardCreatedEvent() override = default;
    DECLARE_KEYBOARD_EVENT(KeyboardCreated);
  };

  class KeyboardDestroyedEvent : public KeyboardEvent { //TODO: probably should refactor this
  public:
    explicit KeyboardDestroyedEvent(const Keyboard* keyboard):
      KeyboardEvent(keyboard) {
    }
    ~KeyboardDestroyedEvent() override = default;
    DECLARE_KEYBOARD_EVENT(KeyboardDestroyed);
  };

  class KeyEvent : public KeyboardEvent {
  protected:
    Key key_;
  public:
    KeyEvent(const Keyboard* keyboard, const Key& key):
      KeyboardEvent(keyboard),
      key_(key) {
    }
    ~KeyEvent() override = default;

    const Key& GetKey() const {
      return key_;
    }

    KeyCode GetKeyCode() const {
      return key_.GetCode();
    }

    KeyEvent* AsKeyEvent() override {
      return this;
    }
  public:
    static inline KeyEvent*
    Cast(KeyboardEvent* event) {
      PRT_ASSERT(event);
      return event->AsKeyEvent();
    }

    static inline std::function<bool(KeyboardEvent*)>
    FilterBy(const KeyCode code) {
      return [code](KeyboardEvent* event) {
        return event
            && event->IsKeyEvent()
            && event->AsKeyEvent()->GetKeyCode() == code;
      };
    }
  };

#define DECLARE_KEYBOARD_KEY_EVENT(Name)                            \
  DECLARE_KEYBOARD_EVENT(Name)                                      \
  public:                                                           \
    static inline std::function<bool(KeyboardEvent*)>               \
    FilterBy(const KeyCode code) {                                  \
      return [code](KeyboardEvent* event) {                         \
        return event                                                \
            && event->Is##Name##Event()                             \
            && event->As##Name##Event()->GetKeyCode() == code;      \
      };                                                            \
    }
  
  class KeyPressedEvent : public KeyEvent {
  public:
    KeyPressedEvent(const Keyboard* keyboard, const Key& key):
      KeyEvent(keyboard, key) {
    }
    ~KeyPressedEvent() override = default;
    DECLARE_KEYBOARD_KEY_EVENT(KeyPressed);
  };

  class KeyReleasedEvent : public KeyEvent {
  public:
    KeyReleasedEvent(const Keyboard* keyboard, const Key& key):
      KeyEvent(keyboard, key) {
    }
    ~KeyReleasedEvent() override = default;
    DECLARE_KEYBOARD_KEY_EVENT(KeyReleased);
  };

  typedef rx::subject<KeyboardEvent*> KeyboardEventSubject;

  class KeyboardEventPublisher : public EventPublisher<KeyboardEvent> {
  protected:
    KeyboardEventPublisher() = default;
    explicit KeyboardEventPublisher(const KeyboardEventSubject& fwd):
      EventPublisher<KeyboardEvent>(fwd) {
    }
  public:
    ~KeyboardEventPublisher() override = default;

#define DEFINE_ON_EVENT(Name)                 \
    inline rx::observable<Name##Event*>       \
    On##Name##Event() {                       \
      return OnEvent()                        \
        .filter(Name##Event::Filter)          \
        .map(Name##Event::Cast);              \
    }
    FOR_EACH_KEYBOARD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

    inline rx::observable<KeyPressedEvent*> OnPressed(const KeyCode code) const {
      return OnEvent()
        .filter(KeyPressedEvent::FilterBy(code))
        .map(KeyPressedEvent::Cast);
    }

    inline rx::observable<KeyReleasedEvent*> OnReleased(const KeyCode code) const {
      return OnEvent()
        .filter(KeyReleasedEvent::FilterBy(code))
        .map(KeyReleasedEvent::Cast);
    }
  };
}

#endif //PRT_KEYBOARD_EVENT_H