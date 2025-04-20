#include "prette/settings_event.h"

#include "prette/to_string.h"

namespace prt {
auto SettingsLoadedEvent::ToString() const -> std::string {
  return ToStringHelper<SettingsLoadedEvent>{};
}

auto SettingsSavedEvent::ToString() const -> std::string {
  return ToStringHelper<SettingsSavedEvent>{};
}
}  // namespace prt