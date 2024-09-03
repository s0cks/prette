#include "prette/mouse/mouse_flags.h"

namespace prt::mouse {
DEFINE_bool(mouse, kDefaultMouseFlagValue, "Enable or disable the Mouse.");
DEFINE_string(cursors_dir, kDefaultCursorsDir, "The directory to load mouse cursors from.");
}