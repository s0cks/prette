#ifndef PRT_STD140_H
#define PRT_STD140_H

#include <type_traits>

// IWYU pragma: begin_exports
#include <cstdint>

#include "prette/glm.h"
// IWYU pragma: end_exports

// #define STD140_MAT4_FORMAT         VK_FORMAT_R32G32_SFLOAT
// #define STD140_MAT4_ALIGNMENT      alignas(8)
// #define STD140_MAT4_TYPE           glm::vec2
#define STD140_MAT4(Name)          alignas(16) glm::mat4 Name

#define STD140_BOOL(Name)          alignas(4) bool Name
#define STD140_UINT32(Name)        alignas(4) uint32_t Name

#define STD140_ALIGNED(Type, Name) STD140_##Type##_ALIGNMENT STD140_##Type##_TYPE Name

#define STD140_VEC2_FORMAT         VK_FORMAT_R32G32_SFLOAT
#define STD140_VEC2_ALIGNMENT      alignas(8)
#define STD140_VEC2_TYPE           glm::vec2
#define STD140_VEC2(Name)          STD140_ALIGNED(VEC2, Name)

#define STD140_U32VEC2_FORMAT      VK_FORMAT_R32G32_UINT
#define STD140_U32VEC2_ALIGNMENT   alignas(8)
#define STD140_U32VEC2_TYPE        glm::u32vec2
#define STD140_U32VEC2(Name)       STD140_ALIGNED(U32VEC2, Name)

#define STD140_VEC3_FORMAT         VK_FORMAT_R32G32B32_SFLOAT
#define STD140_VEC3_ALIGNMENT      alignas(16)
#define STD140_VEC3_TYPE           glm::vec3
#define STD140_VEC3(Name)          STD140_ALIGNED(VEC3, Name)

#define STD140_VEC4_FORMAT         VK_FORMAT_R32G32B32A32_SFLOAT
#define STD140_VEC4_ALIGNMENT      alignas(16)
#define STD140_VEC4_TYPE           glm::vec4
#define STD140_VEC4(Name)          STD140_ALIGNED(VEC4, Name)

#define STD140_SAMPLER_FORMAT      VK_FORMAT_SAMPLER

#define STD140_MATERIAL_ALIGNMENT  alignas(4)
#define STD140_MATERIAL_TYPE       uint32_t
#define STD140_MATERIAL(Name)      STD140_ALIGNED(MATERIAL, Name)

namespace prt::std140 {
template <typename T>
struct is_aligned : public std::false_type {};

template <typename T>
concept IsAligned = is_aligned<T>::value;
}  // namespace prt::std140

#endif  // PRT_STD140_H
