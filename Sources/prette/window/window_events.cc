#include "prette/window/window_events.h"

#include <sstream>
#include "prette/window/window.h"

namespace prt::window {
  auto WindowOpenedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowOpenedEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }

  auto WindowClosedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowClosedEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }

  auto WindowPosEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowPosEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }

  auto WindowSizeEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowSizeEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << "size=" << GetSize();
    ss << ")";
    return ss.str();
  }

  auto WindowFocusEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowFocusEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }

  auto WindowIconifyEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowIconifyEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }

  auto WindowRefreshEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowRefreshEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }

  auto WindowMaximizeEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowMaximizeEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }

  auto WindowContentScaleEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "WindowContentScaleEvent(";
    ss << "window=" << GetWindow()->ToString();
    ss << ")";
    return ss.str();
  }
}