#ifndef PRT_FREETYPE_H
#define PRT_FREETYPE_H

// IWYU pragma: begin_exports
#include <freetype/config/ftheader.h>
#include FT_FREETYPE_H
#include <freetype/freetype.h>
#include <freetype/ftimage.h>
#include <freetype/fttypes.h>
// IWYU pragma: end_exports

#include "prette/platform.h"

namespace prt {
static inline auto CalcBufferSize(const FT_Bitmap& rhs, const int num_channels = 4) -> uint64_t {
  return rhs.width * rhs.rows * num_channels;
}
}  // namespace prt

#endif  // PRT_FREETYPE_H
