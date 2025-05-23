#ifndef PRT_MOUSE_FLAGS_H
#define PRT_MOUSE_FLAGS_H

#include <gflags/gflags.h>

namespace prt {
DECLARE_bool(mouse_scroll_inverted);

static inline auto IsMouseScrollInverted() -> bool {
  return FLAGS_mouse_scroll_inverted;
}
}  // namespace prt

#endif  // PRT_MOUSE_FLAGS_H
