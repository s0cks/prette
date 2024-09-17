#include "prette/settings/settings_events.h"
#include <sstream>

namespace prt::settings {
  auto SettingsLoadingEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "SettingsLoadingEvent(";
    ss << ")";
    return ss.str();
  }

  auto SettingsLoadedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "SettingsLoadedEvent(";
    ss << ")";
    return ss.str();
  }

  auto SettingsSavingEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "SettingsSavingEvent(";
    ss << ")";
    return ss.str();
  }

  auto SettingsSavedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "SettingsSavedEvent(";
    ss << ")";
    return ss.str();
  }
}