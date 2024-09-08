#include "prette/keyboard/keyboard_flags.h"

namespace prt::keyboard {
  static constexpr const auto kFlagKeyboardHelp = "Configure the Keyboard input."
                                                  " Valid values are: [ '', 'disabled', '/path/to/config.json' ]";
  DEFINE_string(keyboard, kFlagKeyboardDefault, kFlagKeyboardHelp); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
}