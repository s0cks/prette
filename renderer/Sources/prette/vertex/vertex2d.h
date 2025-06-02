#ifndef PRT_VERTEX2D_H
#define PRT_VERTEX2D_H

#include <functional>

#include "prette/common.h"
#include "prette/std140.h"
#include "prette/vertex/vertex.h"

namespace prt {
// clang-format off
#define FOR_EACH_BARE2D_ATTR(V) \
  V(0, pos, STD140_VEC2_TYPE, STD140_VEC2_FORMAT, STD140_VEC2_ALIGNMENT)
// clang-format on

DECLARE_VERTEX_CLASS(bare2d, FOR_EACH_BARE2D_ATTR, 1);

// TODO: use Color
#define FOR_EACH_COLOR2D_ATTR(V)                                         \
  V(0, pos, STD140_VEC2_TYPE, STD140_VEC2_FORMAT, STD140_VEC2_ALIGNMENT) \
  V(1, color, STD140_VEC4_TYPE, STD140_VEC4_FORMAT, STD140_VEC4_ALIGNMENT)

DECLARE_VERTEX_CLASS(color2d, FOR_EACH_COLOR2D_ATTR, 2);

#define FOR_EACH_MAT2D_ATTRIBUTE(V)                                           \
  V(0, pos, STD140_VEC2_TYPE, VK_FORMAT_R32G32_SFLOAT, STD140_VEC2_ALIGNMENT) \
  V(1, material, STD140_MATERIAL_TYPE, VK_FORMAT_R32_UINT, STD140_MATERIAL_ALIGNMENT)

DECLARE_VERTEX_CLASS(mat2d, FOR_EACH_MAT2D_ATTRIBUTE, 2);

#define FOR_EACH_TEX2D_ATTR(V)                                           \
  V(0, pos, STD140_VEC2_TYPE, STD140_VEC2_FORMAT, STD140_VEC2_ALIGNMENT) \
  V(1, uv, STD140_VEC2_TYPE, STD140_VEC2_FORMAT, STD140_VEC2_ALIGNMENT)

DECLARE_VERTEX_CLASS(tex2d, FOR_EACH_TEX2D_ATTR, 2);
}  // namespace prt

namespace std {
template <>
struct hash<prt::bare2d::Vertex> : prt::Hasher {
  auto operator()(const prt::bare2d::Vertex& rhs) const -> size_t {
    size_t hash = prt::kInvalidHash;
    Combine(hash, rhs.pos);
    return hash;
  }
};

template <>
struct hash<prt::color2d::Vertex> : prt::Hasher {
  auto operator()(const prt::color2d::Vertex& rhs) const -> size_t {
    size_t hash = prt::kInvalidHash;
    Combine(hash, rhs.pos);
    Combine(hash, rhs.color);
    return hash;
  }
};

template <>
struct hash<prt::mat2d::Vertex> : prt::Hasher {
  auto operator()(const prt::mat2d::Vertex& rhs) const -> size_t {
    size_t hash = prt::kInvalidHash;
    Combine(hash, rhs.pos);
    Combine(hash, rhs.material);
    return hash;
  }
};

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

#endif  // PRT_VERTEX2D_H
