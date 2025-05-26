#ifndef PRT_VERTEX_MAT2D_H
#define PRT_VERTEX_MAT2D_H

#ifndef PRT_VERTEX2D_H
#error "Please #include <prette/vertex/vertex2d.h> instead."
#endif  // PRT_VERTEX2D_H

#include <functional>

#include "prette/common.h"
#include "prette/vertex/vertex.h"
#include "prette/vk.h"

namespace prt {
#define FOR_EACH_MAT2D_ATTRIBUTE(V)                                           \
  V(0, pos, STD140_VEC2_TYPE, VK_FORMAT_R32G32_SFLOAT, STD140_VEC2_ALIGNMENT) \
  V(1, material, STD140_MATERIAL_TYPE, VK_FORMAT_R32_UINT, STD140_MATERIAL_ALIGNMENT)

DECLARE_VERTEX_CLASS(mat2d, FOR_EACH_MAT2D_ATTRIBUTE, 2);
}  // namespace prt

namespace std {
template <>
struct hash<prt::mat2d::Vertex> : prt::Hasher {
  auto operator()(const prt::mat2d::Vertex& rhs) const -> size_t {
    size_t hash = prt::kInvalidHash;
    Combine(hash, rhs.pos);
    Combine(hash, rhs.material);
    return hash;
  }
};
}  // namespace std

#endif  // PRT_VERTEX_MAT2D_H
