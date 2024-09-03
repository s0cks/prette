#include "prette/settings/setting_events.h"
#include <sstream>

namespace prt::settings {
  std::string SettingChangedEvent::ToString() const {
    std::stringstream ss;
    ss << "SettingChangedEvent(";
    ss << ")";
    return ss.str();
  }
}