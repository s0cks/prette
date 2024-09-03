#include "prette/gui/gui_events.h"

#include <sstream>
#include "prette/gui/gui_window.h"

namespace prt::gui {
  std::string WindowOpenedEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowOpenedEvent(";
    ss << "window=" << GetWindow();
    ss << ")";
    return ss.str();
  }

  std::string WindowClosedEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowClosedEvent(";
    ss << "window=" << GetWindow();
    ss << ")";
    return ss.str();
  }

  std::string MouseExitEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseExitEvent(";
    ss << "pos=" << glm::to_string(GetPos());
    ss << ")";
    return ss.str();
  }

  std::string MouseEnterEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseEnterEvent(";
    ss << "pos=" << glm::to_string(GetPos());
    ss << ")";
    return ss.str();
  }

  std::string MouseClickEvent::ToString() const {
    std::stringstream ss;
    ss << "MouseClickEvent(";
    ss << "pos=" << glm::to_string(GetPos());
    ss << ")";
    return ss.str();
  }

  std::string RootComponentRegisteredEvent::ToString() const {
    std::stringstream ss;
    ss << "RootComponentRegisteredEvent(";
    ss << "component=" << GetComponent();
    ss << ")";
    return ss.str();
  }

  std::string RootComponentRemovedEvent::ToString() const {
    std::stringstream ss;
    ss << "RootComponentRemovedEvent(";
    ss << "component=" << GetComponent();
    ss << ")";
    return ss.str();
  }
}