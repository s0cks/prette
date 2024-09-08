#ifndef PRT_KEYBOARD_FLAGS_H
#define PRT_KEYBOARD_FLAGS_H

#include <gflags/gflags.h>

namespace prt::keyboard {
  static constexpr const auto kFlagKeyboardDefault = "";
  static constexpr const auto kFlagKeyboardDisabled = "disabled";
  DECLARE_string(keyboard); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static inline auto
  IsKeyboardEnabled() -> bool {
    return strncasecmp(FLAGS_keyboard.data(), kFlagKeyboardDisabled, 8) != 0;
  }

  static inline auto
  IsKeyboardDisabled() -> bool {
    return strncasecmp(FLAGS_keyboard.data(), kFlagKeyboardDisabled, 8) == 0;
  }

  static inline auto
  HasKeyboardConfig() -> bool {
    return !FLAGS_keyboard.empty();
  }
}

#endif //PRT_KEYBOARD_FLAGS_H