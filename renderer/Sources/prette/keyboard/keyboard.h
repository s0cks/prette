#ifndef PRT_KEYBOARD_H
#define PRT_KEYBOARD_H

#include <functional>
#include <ostream>
#include <sstream>
#include <string>

#include "prette/common.h"
#include "prette/event.h"
#include "prette/to_string.h"

#ifdef PRT_GLFW
#include <GLFW/glfw3.h>
#endif  // PRT_GLFW

namespace prt {
class KeyState {
  DEFINE_DEFAULT_COPYABLE_TYPE(KeyState);

 private:
  int code_;
  int state_;

 public:
  KeyState() = default;
  KeyState(const int code, const int state) :
    code_(code),
    state_(state) {}
  ~KeyState() = default;

  auto GetCode() const {
    return code_;
  }

  auto GetState() const -> int {
    return state_;
  }

#ifdef PRT_GLFW
  inline auto IsRepeat() const -> bool {
    return GetState() == GLFW_REPEAT;
  }

  inline auto IsPressed() const -> bool {
    return GetState() == GLFW_PRESS;
  }

  inline auto IsReleased() const -> bool {
    return GetState() == GLFW_RELEASE;
  }
#endif  // PRT_GLFW

  auto ToString() const -> std::string {
    ToStringHelper<KeyState> helper{};
    helper.AddFieldRef("code", GetCode());
    if (IsPressed()) {
      helper.AddField("state", "pressed");
    } else if (IsReleased()) {
      helper.AddField("state", "released");
    } else if (IsRepeat()) {
      helper.AddField("state", "repeat");
    } else {
      std::stringstream ss;
      ss << "unknown state: " << GetState();
      helper.AddField("state", ss);
    }
    return helper;
  }

  auto operator==(const KeyState& rhs) const -> bool {
    return code_ == rhs.code_ && state_ == rhs.state_;
  }

  auto operator!=(const KeyState& rhs) const -> bool {
    return code_ != rhs.code_ || state_ != rhs.state_;
  }

  friend auto operator<<(std::ostream& stream, const KeyState& rhs) -> std::ostream& {
    if (rhs.IsPressed()) {
      return stream << "pressed";
    } else if (rhs.IsReleased()) {
      return stream << "release";
    } else if (rhs.IsRepeat()) {
      return stream << "repeat";
    }
    return stream << "unknown state";
  }
};

#define FOR_EACH_KEYBOARD_EVENT(V) \
  V(KeyboardInit)                  \
  V(KeyState)                      \
  V(KeyboardDestroyed)

class Keyboard;
class KeyboardEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_KEYBOARD_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Keyboard, FOR_EACH_KEYBOARD_EVENT);

class KeyboardEventBase : public KeyboardEvent {
 private:
  const Keyboard* keyboard_;

 protected:
  explicit KeyboardEventBase(const Keyboard* keyboard) :
    KeyboardEvent(),
    keyboard_(keyboard) {
    ASSERT(keyboard_);
  }

 public:
  ~KeyboardEventBase() override = default;

  auto GetKeyboard() const -> const Keyboard* {
    return keyboard_;
  }
};

class KeyboardInitEvent : public KeyboardEventBase {
 public:
  explicit KeyboardInitEvent(const Keyboard* keyboard) :
    KeyboardEventBase(keyboard) {}
  ~KeyboardInitEvent() override = default;
  DECLARE_EVENT_TYPE(KeyboardEvent, KeyboardInit);
};

class KeyboardDestroyedEvent : public KeyboardEvent {
 public:
  KeyboardDestroyedEvent() = default;
  ~KeyboardDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(KeyboardEvent, KeyboardDestroyed);
};

class KeyStateEvent : public KeyboardEventBase {
 private:
  KeyState state_;
  int mods_;
  int scancode_;

 public:
  KeyStateEvent(const Keyboard* keyboard, const int key, const int scancode, const int action, const int mods) :
    KeyboardEventBase(keyboard),
    state_(key, action),
    mods_(mods),
    scancode_(scancode) {}
  ~KeyStateEvent() override = default;

  auto GetState() const -> const KeyState& {
    return state_;
  }

  auto GetCode() const -> int {
    return state_.GetCode();
  }

  inline auto IsCode(const int code) const -> bool {
    return GetCode() == code;
  }

  auto GetMods() const -> int {
    return mods_;
  }

  auto GetScanCode() const -> int {
    return scancode_;
  }

  inline auto IsRepeat() const -> bool {
    return state_.IsRepeat();
  }

  inline auto IsRepeat(const int code) -> bool {
    return IsRepeat() && GetCode() == code;
  }

  inline auto IsReleased() const -> bool {
    return state_.IsReleased();
  }

  inline auto IsReleased(const int code) -> bool {
    return IsReleased() && GetCode() == code;
  }

  inline auto IsPressed() const -> bool {
    return state_.IsPressed();
  }

  inline auto IsPressed(const int code) -> bool {
    return IsPressed() && GetCode() == code;
  }

  auto GetKey() const -> const char*;

  auto IsState(const KeyState& rhs) const -> bool {
    return state_ == rhs;
  }

#ifdef PRT_ENABLE_LUA
  void ToTable(lua_State* L) const override;
#endif  // PRT_ENABLE_LUA
  DECLARE_EVENT_TYPE(KeyboardEvent, KeyState);

 public:
  static inline auto FilterBy(const KeyState& state) -> KeyboardEvent::Predicate {
    return [=](KeyboardEvent* event) {
      return event && event->IsKeyStateEvent() && event->AsKeyStateEvent()->IsState(state);
    };
  }

  static inline auto FilterByPressed() -> KeyboardEvent::Predicate {
    return [](KeyboardEvent* event) {
      return event && event->IsKeyStateEvent() && event->AsKeyStateEvent()->IsPressed();
    };
  }

#ifdef PRT_GLFW
  static inline auto FilterByPressed(const int code) -> std::function<bool(KeyboardEvent*)> {
    return FilterBy(KeyState(code, GLFW_PRESS));
  }

  static inline auto FilterByReleased(const int code) -> KeyboardEvent::Predicate {
    return FilterBy(KeyState(code, GLFW_RELEASE));
  }
#endif  // PRT_GLFW

  static inline auto FilterByReleased() -> KeyboardEvent::Predicate {
    return [](KeyboardEvent* event) {
      return event && event->IsKeyStateEvent() && event->AsKeyStateEvent()->IsReleased();
    };
  }
};

DEFINE_EVENT_SUBJECT(Keyboard);
DEFINE_EVENT_OBSERVABLE(Keyboard);
FOR_EACH_KEYBOARD_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnKeyboardEvent() -> KeyboardEventObservable;
#define DEFINE_ON_EVENT(Name)                                                    \
  static inline auto On##Name##Event()->Name##EventObservable {                  \
    return OnKeyboardEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_KEYBOARD_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

static inline auto OnKeyPressed() -> KeyStateEventObservable {
  return OnKeyboardEvent().filter(KeyStateEvent::FilterByPressed()).map(KeyStateEvent::Cast);
}

static inline auto OnKeyPressed(const int code) -> KeyStateEventObservable {
  return OnKeyboardEvent().filter(KeyStateEvent::FilterByPressed(code)).map(KeyStateEvent::Cast);
}

static inline auto OnKeyReleased() -> KeyStateEventObservable {
  return OnKeyboardEvent().filter(KeyStateEvent::FilterByReleased()).map(KeyStateEvent::Cast);
}

static inline auto OnKeyReleased(const int code) -> KeyStateEventObservable {
  return OnKeyboardEvent().filter(KeyStateEvent::FilterByReleased(code)).map(KeyStateEvent::Cast);
}

class Window;
class Keyboard : public EventSourceTemplate<KeyboardEvent> {
#ifdef PRT_GLFW
  static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
  static auto GetKeyboard(GLFWwindow* handle) -> Keyboard*;
#endif  // PRT_GLFW
 private:
  Window* owner_;

  void PublishEvent(KeyboardEvent* event) const override;

  template <class E, typename... Args>
  void Publish(Args... args) const {
    E event(args...);
    return PublishEvent(&event);
  }

 public:
  Keyboard(Window* owner);
  ~Keyboard() override;

  auto GetOwner() const -> Window* {
    return owner_;
  }

  inline auto HasOwner() const -> bool {
    return GetOwner() != nullptr;
  }

  auto GetKey(const int code) const -> KeyState;
  auto ToString() const -> std::string;

 private:
  static auto New(Window* owner) -> Keyboard*;
#ifdef PRT_ENABLE_LUA
  static void InitLua(lua_State* L);
#endif  // PRT_ENABLE_LUA

 public:
  static void Init(Window* window);
  static auto IsInitialized() -> bool;
  static auto Get() -> Keyboard*;
};
}  // namespace prt

#endif  // PRT_KEYBOARD_H
