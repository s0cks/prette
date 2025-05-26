#ifndef PRT_MOUSE_SETTINGS_H
#define PRT_MOUSE_SETTINGS_H

#include "prette/platform.h"
#include "prette/settings/setting_type.h"

namespace prt {
struct MouseScrollInvertedSetting : public BoolSetting {
  static constexpr const auto kSettingName = "MouseScrollInverted";
#ifdef OS_IS_OSX
  static constexpr const auto kDefaultValue = true;
#else
  static constexpr const auto kDefaultValue = false
#endif  // OS_IS_OSX
};

class MouseSettings {
  friend class Mouse;
  friend class MouseSettings;

 private:
  bool inverted = MouseScrollInvertedSetting::kDefaultValue;

 public:
  MouseSettings() = default;
  ~MouseSettings() = default;

  auto IsInverted() const -> bool {
    return inverted;
  }
};
}  // namespace prt

#endif  // PRT_MOUSE_SETTINGS_H
