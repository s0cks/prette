#include "prette/window/monitor.h"

namespace prt::window {
  auto Monitor::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Monitor(";
    ss << "name=" << GetName() << ", ";
    ss << "resolution=" << GetResolution() << ", ";
    ss << "refresh_rate=" << GetRefreshRateInHertz();
    ss << ")";
    return ss.str();
  }
}