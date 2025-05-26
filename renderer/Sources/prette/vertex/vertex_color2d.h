#ifndef PRT_VERTEX_COLOR2D_H
#define PRT_VERTEX_COLOR2D_H

#ifndef PRT_VERTEX2D_H
#error "Please #include <prette/vertex/vertex2d.h> instead."
#endif  // PRT_VERTEX2D_H

#include <functional>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/vertex/vertex.h"

namespace prt {
// TODO: use Color
#define FOR_EACH_COLOR2D_ATTR(V)                                         \
  V(0, pos, STD140_VEC2_TYPE, STD140_VEC2_FORMAT, STD140_VEC2_ALIGNMENT) \
  V(1, color, STD140_VEC4_TYPE, STD140_VEC4_FORMAT, STD140_VEC4_ALIGNMENT)

DECLARE_VERTEX_CLASS(color2d, FOR_EACH_COLOR2D_ATTR, 2);
}  // namespace prt

namespace std {
template <>
struct hash<prt::color2d::Vertex> : prt::Hasher {
  auto operator()(const prt::color2d::Vertex& rhs) const -> size_t {
    size_t hash = prt::kInvalidHash;
    Combine(hash, rhs.pos);
    Combine(hash, rhs.color);
    return hash;
  }
};
}  // namespace std

#endif  // PRT_VERTEX_COLOR2D_H
