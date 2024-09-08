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
  DECLARE_EVENT_TYPE(KeyboardEvent, Name)

  class KeyboardEvent : public Event {
    DEFINE_NON_COPYABLE_TYPE(KeyboardEvent);
  private:
    const Keyboard* keyboard_;
  protected:
    explicit KeyboardEvent(const Keyboard* keyboard):
      Event(),
      keyboard_(keyboard) {
    }
  public:
    ~KeyboardEvent() override = default;

    auto keyboard() const -> const Keyboard* {
      return keyboard_;
    }


    virtual auto AsKeyEvent() -> KeyEvent* {
      return nullptr;
    }

    auto IsKeyEvent() -> bool {
      return AsKeyEvent() != nullptr;
    }

#define DEFINE_TYPE_CHECK(Name)                                               \
    virtual auto As##Name##Event() -> Name##Event* { return nullptr; }        \
    auto Is##Name##Event() -> bool { return As##Name##Event() != nullptr; }
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
    DEFINE_NON_COPYABLE_TYPE(KeyEvent);
  private:
    Key key_;
  public:
    KeyEvent(const Keyboard* keyboard, const Key& key):
      KeyboardEvent(keyboard),
      key_(key) {
    }
    ~KeyEvent() override = default;

    auto GetKey() const -> const Key& {
      return key_;
    }

    auto GetKeyCode() const -> KeyCode {
      return key_.GetCode();
    }

    auto AsKeyEvent() -> KeyEvent* override {
      return this;
    }
  public:
    static inline auto
    Cast(KeyboardEvent* event) -> KeyEvent* {
      PRT_ASSERT(event);
      return event->AsKeyEvent();
    }

    static inline auto
    FilterBy(const KeyCode code) -> std::function<bool(KeyboardEvent*)> {
      return [code](KeyboardEvent* event) {
        return event
            && event->IsKeyEvent()
            && event->AsKeyEvent()->GetKeyCode() == code;
      };
    }
  };

#define DECLARE_KEYBOARD_KEY_EVENT(Name)                                    \
  DECLARE_KEYBOARD_EVENT(Name)                                              \
  public:                                                                   \
    static inline auto                                                      \
    FilterBy(const KeyCode code) -> std::function<bool(KeyboardEvent*)> {   \
      return [code](KeyboardEvent* event) {                                 \
        return event                                                        \
            && event->Is##Name##Event()                                     \
            && event->As##Name##Event()->GetKeyCode() == code;              \
      };                                                                    \
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

  DEFINE_EVENT_SUBJECT(Keyboard);
  DEFINE_EVENT_OBSERVABLE(Keyboard);
  FOR_EACH_KEYBOARD_EVENT(DEFINE_EVENT_OBSERVABLE);

  class KeyboardEventSource : public EventSource<KeyboardEvent> {
    DEFINE_NON_COPYABLE_TYPE(KeyboardEventSource);
  protected:
    KeyboardEventSource() = default;
  public:
    ~KeyboardEventSource() override = default;
#define DEFINE_ON_EVENT(Name)                           \
    auto On##Name() const -> Name##EventObservable {    \
      return OnEvent()                                  \
        .filter(Name##Event::Filter)                    \
        .map(Name##Event::Cast);                        \
    }
    FOR_EACH_KEYBOARD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };
}

#endif //PRT_KEYBOARD_EVENT_H