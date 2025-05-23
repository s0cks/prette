#ifndef PRT_VERTEX_TEX2D_H
#define PRT_VERTEX_TEX2D_H

#ifndef PRT_VERTEX2D_H
#error "Please #include <prette/vertex2d.h> instead."
#endif  // PRT_VERTEX2D_H

#include <functional>
#include <type_traits>
#include <vulkan/vulkan_core.h>

#include "prette/color.h"  // IWYU pragma: keep
#include "prette/common.h"
#include "prette/vertex.h"
#include "prette/vertex_buffer.h"  // IWYU pragma: keep

namespace prt {
#define FOR_EACH_TEX2D_ATTR(V)                                           \
  V(0, pos, STD140_VEC2_TYPE, STD140_VEC2_FORMAT, STD140_VEC2_ALIGNMENT) \
  V(1, uv, STD140_VEC2_TYPE, STD140_VEC2_FORMAT, STD140_VEC2_ALIGNMENT)

DECLARE_VERTEX_CLASS(tex2d, FOR_EACH_TEX2D_ATTR, 2);

template <>
struct is_2d_vertex_t<tex2d::Vertex> : std::true_type {};
}  // namespace prt

namespace std {
template <>
struct hash<prt::tex2d::Vertex> : prt::Hasher {
  auto operator()(const prt::tex2d::Vertex& rhs) const -> size_t {
    size_t hash = prt::kInvalidHash;
    Combine(hash, rhs.pos);
    Combine(hash, rhs.uv);
    return hash;
  }
};
}  // namespace std

#endif  // PRT_VERTEX_TEX2D_H
