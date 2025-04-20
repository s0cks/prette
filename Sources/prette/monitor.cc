#include "prette/monitor.h"

namespace prt {
auto MonitorConnectedEvent::ToString() const -> std::string {
  std::stringstream ss;
  ss << "MonitorConnectedEvent(";
  ss << "monitor=" << GetMonitor()->ToString();
  ss << ")";
  return ss.str();
}

auto MonitorDisconnectedEvent::ToString() const -> std::string {
  std::stringstream ss;
  ss << "MonitorDisconnectedEvent(";
  ss << "monitor=" << GetMonitor()->ToString();
  ss << ")";
  return ss.str();
}

auto Monitor::ToString() const -> std::string {
  std::stringstream ss;
  ss << "Monitor(";
  ss << "name=" << GetName() << ", ";
  ss << "resolution=" << GetResolution() << ", ";
  ss << "refresh_rate=" << GetRefreshRateInHertz();
  ss << ")";
  return ss.str();
}
}  // namespace prt