#include "prette/settings/setting_events.h"
#include <sstream>

namespace prt::settings {
  auto SettingChangedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "SettingChangedEvent(";
    ss << ")";
    return ss.str();
  }
}