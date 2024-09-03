#include "prette/settings/settings_events.h"
#include <sstream>

namespace prt::settings {
  std::string SettingsLoadingEvent::ToString() const {
    std::stringstream ss;
    ss << "SettingsLoadingEvent(";
    ss << ")";
    return ss.str();
  }

  std::string SettingsLoadedEvent::ToString() const {
    std::stringstream ss;
    ss << "SettingsLoadedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string SettingsSavingEvent::ToString() const {
    std::stringstream ss;
    ss << "SettingsSavingEvent(";
    ss << ")";
    return ss.str();
  }

  std::string SettingsSavedEvent::ToString() const {
    std::stringstream ss;
    ss << "SettingsSavedEvent(";
    ss << ")";
    return ss.str();
  }
}