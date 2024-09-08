#ifndef PRT_KEYBOARD_FLAGS_H
#define PRT_KEYBOARD_FLAGS_H

#include <gflags/gflags.h>
#include "prette/keyboard/keyboard_mode.h"

namespace prt::keyboard {
DECLARE_string(keyboard_mode); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static inline auto
  GetKeyboardMode() -> std::optional<Mode> {
    return ParseMode(FLAGS_keyboard_mode);
  }
}

#endif //PRT_KEYBOARD_FLAGS_H