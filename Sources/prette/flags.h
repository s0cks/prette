#ifndef PRT_FLAGS_H
#define PRT_FLAGS_H

// IWYU pragma: begin_exports
#include <gflags/gflags.h>
// IWYU pragma: end_exports

namespace prt {
DECLARE_bool(gfx);

#ifdef PRT_DEBUG
DECLARE_string(resources);
#endif  // PRT_DEBUG
}  // namespace prt

#endif  // PRT_FLAGS_H