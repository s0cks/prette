#ifndef PRT_GLM_H
#define PRT_GLM_H

// IWYU pragma: begin_exports
#include <cmath>  //IWYU pragma: keep
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_LEFT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_uint4_sized.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
// IWYU pragma: end_exports

#include <vulkan/vulkan_core.h>

namespace prt {
template <typename>
struct is_glm_type {
  static constexpr const auto value = false;
};

#define DEFINE_IS_GLM_TYPE(Name)              \
  template <>                                 \
  struct is_glm_type<Name> {                  \
    static constexpr const auto value = true; \
  };
DEFINE_IS_GLM_TYPE(glm::vec2);
DEFINE_IS_GLM_TYPE(glm::vec3);
DEFINE_IS_GLM_TYPE(glm::mat4);
#undef DEFINE_IS_GLM_TYPE

static inline auto Clamp(glm::vec2& pos, const glm::vec4& bounds) -> bool {
  bool changed = false;
  if (pos.x < bounds[0]) {
    pos.x = bounds[0];
    changed = true;
  } else if (pos.x > bounds[2]) {
    pos.x = bounds[2];
    changed = true;
  } else if (pos.y < bounds[1]) {
    pos.y = bounds[1];
    changed = true;
  } else if (pos.y > bounds[3]) {
    pos.y = bounds[3];
    changed = true;
  }
  return changed;
}

static inline auto Clamp(glm::vec2& pos, const glm::vec2& size) -> bool {
  return Clamp(pos, glm::vec4(0.0f, 0.0f, size));
}

static inline auto ToNormalizedDeviceCoords(const glm::vec2& pos, const glm::vec2& viewport_size) -> glm::vec2 {
  return {(pos.x / viewport_size.x * 2.0f) - 1.0f, (pos.y / viewport_size.y * 2.0f) - 1.0f};
}
}  // namespace prt

namespace glm {
// TODO: move to client package
static inline auto to_vec2(const VkExtent2D& rhs) -> u32vec2 {
  return {rhs.width, rhs.height};
}
}  // namespace glm

#endif  // PRT_GLM_H
