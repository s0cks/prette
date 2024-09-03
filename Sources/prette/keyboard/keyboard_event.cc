#include "prette/keyboard/keyboard_event.h"
#include <sstream>

namespace prt::keyboard {
  std::string KeyboardCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "KeyboardCreatedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string KeyboardDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "KeyboardDestroyedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string KeyPressedEvent::ToString() const {
    std::stringstream ss;
    ss << "KeyPressedEvent(";
    ss << "key=" << GetKey();
    ss << ")";
    return ss.str();
  }

  std::string KeyReleasedEvent::ToString() const {
    std::stringstream ss;
    ss << "KeyReleasedEvent(";
    ss << "key=" << GetKey();
    ss << ")";
    return ss.str();
  }
}