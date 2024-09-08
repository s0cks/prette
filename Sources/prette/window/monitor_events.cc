#include "prette/window/monitor_events.h"

namespace prt::window {
  auto MonitorConnectedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "MonitorConnectedEvent(";
    ss << "monitor=" << GetMonitor();
    ss << ")";
    return ss.str();
  }

  auto MonitorDisconnectedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "MonitorDisconnectedEvent(";
    ss << "monitor=" << GetMonitor();
    ss << ")";
    return ss.str();
  }
}