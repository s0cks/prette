#ifndef PRT_KEYBOARD_FLAGS_H
#define PRT_KEYBOARD_FLAGS_H

#include "prette/flags.h"

namespace prt::keyboard {
  static constexpr const auto kFlagKeyboardDefault = "";
  static constexpr const auto kFlagKeyboardDisabled = "disabled";
  DECLARE_string(keyboard);

  static inline bool
  IsKeyboardEnabled() {
    return strncasecmp(FLAGS_keyboard.data(), kFlagKeyboardDisabled, 8) != 0;
  }

  static inline bool
  IsKeyboardDisabled() {
    return strncasecmp(FLAGS_keyboard.data(), kFlagKeyboardDisabled, 8) == 0;
  }

  static inline bool
  HasKeyboardConfig() {
    return !FLAGS_keyboard.empty();
  }
}

#endif //PRT_KEYBOARD_FLAGS_H