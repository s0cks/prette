#include "prette/window/window_events.h"

#include <sstream>

namespace prt {
  std::string WindowOpenedEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowOpenedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string WindowClosedEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowClosedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string WindowPosEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowPosEvent(";
    ss << ")";
    return ss.str();
  }

  std::string WindowSizeEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowSizeEvent(";
    ss << "size=" << size();
    ss << ")";
    return ss.str();
  }

  std::string WindowFocusEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowFocusEvent(";
    ss << ")";
    return ss.str();
  }

  std::string WindowIconifyEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowIconifyEvent(";
    ss << ")";
    return ss.str();
  }

  std::string WindowRefreshEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowRefreshEvent(";
    ss << ")";
    return ss.str();
  }

  std::string WindowMaximizeEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowMaximizeEvent(";
    ss << ")";
    return ss.str();
  }

  std::string WindowContentScaleEvent::ToString() const {
    std::stringstream ss;
    ss << "WindowContentScaleEvent(";
    ss << ")";
    return ss.str();
  }
}