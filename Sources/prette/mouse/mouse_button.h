#ifndef PRT_MOUSE_BUTTON_H
#define PRT_MOUSE_BUTTON_H

#include <set>
#include <array>
#include <bitset>

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt::mouse {
#define FOR_EACH_MOUSE_BUTTON_STATE(V)    \
  V(Released,       GLFW_RELEASE)         \
  V(Pressed,        GLFW_PRESS)

  static constexpr const auto kNumberOfMouseButtons = GLFW_MOUSE_BUTTON_LAST;

  typedef std::bitset<kNumberOfMouseButtons> MouseButtonBitset;                    

  class MouseButton {
    friend class Mouse;
    friend class GlfwMouse;
  public:
    typedef int8_t Code;

    enum State : int8_t {
#define DEFINE_STATE(Name, Value) k##Name = (Value),
      FOR_EACH_MOUSE_BUTTON_STATE(DEFINE_STATE)
#undef DEFINE_STATE
    };

    friend std::ostream& operator<<(std::ostream& stream, const State& rhs) {
      switch(rhs) {
#define DEFINE_TO_STRING(Name, Value) \
        case k##Name: return stream << #Name << " (" << Value << ")";
        FOR_EACH_MOUSE_BUTTON_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
        default: return stream << "Unknown MouseButton::State";
      }
    }
  protected:
    Code code_;

    State GetState(GLFWwindow* handle) const;
  public:
    explicit constexpr MouseButton(const Code code):
      code_(code) {
    }
    MouseButton(const MouseButton& rhs) = default;
    ~MouseButton() = default;

    Code GetCode() const {
      return code_;
    }

    operator Code () const {
      return code_;
    }

    MouseButton& operator=(const MouseButton& rhs) = default;

    bool operator==(const MouseButton& rhs) const {
      return GetCode() == rhs.GetCode();
    }

    bool operator!=(const MouseButton& rhs) const {
      return GetCode() != rhs.GetCode();
    }

    bool operator<(const MouseButton& rhs) const {
      return GetCode() < rhs.GetCode();
    }

    bool operator>(const MouseButton& rhs) const {
      return GetCode() > rhs.GetCode();
    }

    friend std::ostream& operator<<(std::ostream& stream, const MouseButton& rhs) {
      stream << "MouseButton(";
      stream << "code=" << rhs.GetCode();
      stream << ")";
      return stream;
    }
  };

  typedef std::set<MouseButton> MouseButtonSet;

  class MouseButtonStateSet {
  protected:
    std::array<MouseButton::State, kNumberOfMouseButtons> states_;
  public:
    MouseButtonStateSet() = default;
    ~MouseButtonStateSet() = default;

    MouseButton::State GetState(const MouseButton::Code code) const {
      PRT_ASSERT(code >= 0 && code <= kNumberOfMouseButtons);
      return states_[code];
    }

#define DEFINE_STATE_CHECK(Name, Value)                             \
    inline bool Is##Name(const MouseButton::Code code) const {      \
      return GetState(code) == MouseButton::k##Name;                \
    }
    FOR_EACH_MOUSE_BUTTON_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

    MouseButton::State& operator[](const MouseButton::Code code) {
      PRT_ASSERT(code >= 0 && code <= kNumberOfMouseButtons);
      return states_[code];
    }

    rx::observable<std::pair<MouseButton, MouseButton::State>> ToObservable() const {
      MouseButton::Code code = 0;
      return rx::observable<>::iterate(states_)
        .map([this,&code](const MouseButton::State& state) {
          return std::make_pair(MouseButton(code++), state);
        });
    }

    operator rx::observable<std::pair<MouseButton, MouseButton::State>> () const {
      return ToObservable();
    }
  };
}

#endif //PRT_MOUSE_BUTTON_H