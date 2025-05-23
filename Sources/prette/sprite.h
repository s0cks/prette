#ifndef PRT_SPRITE_H
#define PRT_SPRITE_H

#include <type_traits>
#include <vulkan/vulkan_core.h>

#include "prette/glm.h"
#include "prette/mesh.h"
#include "prette/uniform_buffer.h"
#include "prette/vertex.h"

namespace prt {
struct SpriteData {
  STD140_MAT4(model) = glm::mat4(1.0f);
  STD140_VEC2(pos) = glm::vec2(0.0f);
  DEFINE_VERTEX_GET_INSTANCE_BINDING_DESCRIPTION(SpriteData);
};

template <>
struct vk::is_mesh_data_t<SpriteData> : std::true_type {};

template <>
struct vk::is_uniform_t<SpriteData> : std::true_type {};
}  // namespace prt

#endif  // PRT_SPRITE_H
