#ifndef PRT_LIGHT_H
#define PRT_LIGHT_H

#include <type_traits>

#include "prette/std140.h"
#include "prette/vertex/vertex.h"

namespace prt {
struct LightData {
  STD140_VEC3(pos);
  STD140_VEC3(color);
};

template <>
struct std140::is_aligned<LightData> : std::true_type {};
}  // namespace prt

#endif  // PRT_LIGHT_H
