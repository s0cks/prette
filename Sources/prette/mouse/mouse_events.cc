#include "prette/mouse/mouse_events.h"
#include <sstream>
#include "prette/mouse/mouse.h"

namespace prt::mouse {
  std::string MouseCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseCreatedEvent(";
    ss << "mouse=" << GetMouse()->ToString();
    ss << ")";
    return ss.str();
  }

  std::string MouseDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseDestroyedEvent(";
    ss << "mouse=" << GetMouse()->ToString();
    ss << ")";
    return ss.str();
  }

  std::string MouseButtonPressedEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent(";
    ss << "mouse=" << GetMouse()->ToString() << ", ";
    ss << "button=" << GetButton() << ", ";
    ss << "pos=" << glm::to_string(GetPos());
    ss << ")";
    return ss.str();
  }

  std::string MouseButtonReleasedEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent(";
    ss << "mouse=" << GetMouse()->ToString() << ", ";
    ss << "button=" << GetButton() << ", ";
    ss << "pos=" << glm::to_string(GetPos());
    ss << ")";
    return ss.str();
  }

  std::string MouseMoveEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseMoveEvent(";
    ss << "pos=" << glm::to_string(pos()) << ", ";
    ss << "last=" << glm::to_string(last_pos()) << ", ";
    ss << "delta=" << glm::to_string(delta());
    ss << ")";
    return ss.str();
  }

#define DEFINE_EVENT_SUBSCRIBER(Name)                             \
  Name##EventSubscriber::Name##EventSubscriber(Mouse* mouse):     \
    sub_() {                                                      \
    PRT_ASSERT(mouse);                                            \
    sub_ = mouse->On##Name##Event()                               \
      .subscribe([this](Name##Event* event) {                     \
        return On##Name(event);                                   \
      });                                                         \
  }
  FOR_EACH_MOUSE_EVENT(DEFINE_EVENT_SUBSCRIBER)
#undef DEFINE_EVENT_SUBSCRIBER
}