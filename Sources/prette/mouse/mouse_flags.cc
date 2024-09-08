#include "prette/mouse/mouse_flags.h"

// NOLINTBEGIN(modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-avoid-non-const-global-variables)
namespace prt::mouse {
  static constexpr const auto kDefaultMouseMode = "enabled";
  static constexpr const auto kMouseModeHelp = "Set the mode for the mouse.";
  DEFINE_string(mouse_mode, kDefaultMouseMode, kMouseModeHelp);

  static constexpr const auto kCursorDirHelp = "The directory to load mouse cursors from.";
  DEFINE_string(cursors_dir, kDefaultCursorsDir, kCursorDirHelp);
}

// NOLINTEND(modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-avoid-non-const-global-variables)