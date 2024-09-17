#include "prette/keyboard/keyboard_event.h"
#include <sstream>

namespace prt::keyboard {
  auto KeyboardCreatedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "KeyboardCreatedEvent(";
    ss << ")";
    return ss.str();
  }

  auto KeyboardDestroyedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "KeyboardDestroyedEvent(";
    ss << ")";
    return ss.str();
  }

  auto KeyPressedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "KeyPressedEvent(";
    ss << "key=" << GetKey();
    ss << ")";
    return ss.str();
  }

  auto KeyReleasedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "KeyReleasedEvent(";
    ss << "key=" << GetKey();
    ss << ")";
    return ss.str();
  }
}