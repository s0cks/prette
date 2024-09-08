#include "prette/keyboard/keyboard_flags.h"

namespace prt::keyboard {
static constexpr const auto kDefaultKeyboardMode = "enabled";
static constexpr const auto kKeyboardModeHelp = "The mode for the Keyboard.";
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  DEFINE_string(keyboard_mode, kDefaultKeyboardMode, kKeyboardModeHelp);
}