#ifndef PRT_FLAGS_H
#define PRT_FLAGS_H

#include <gflags/gflags.h>

namespace prt {
DECLARE_bool(gfx);

#ifdef PRT_DEBUG
DECLARE_string(resources);
#endif  // PRT_DEBUG
}  // namespace prt

#endif  // PRT_FLAGS_H