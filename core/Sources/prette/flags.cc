#include "prette/flags.h"

namespace prt {
DEFINE_string(resources, "", "The folder where resources are stored.");

#ifdef PRT_DEBUG
DEFINE_bool(gfx, true, "Disable the graphics");
#endif  // PRT_DEBUG
}  // namespace prt