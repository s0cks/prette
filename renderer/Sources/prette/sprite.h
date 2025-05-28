#ifndef PRT_SPRITE_H
#define PRT_SPRITE_H

#include <vulkan/vulkan_core.h>

#include "prette/glm.h"
#include "prette/vertex/vertex.h"

namespace prt {
struct SpriteData {
  STD140_MAT4(model) = glm::mat4(1.0f);
  STD140_VEC2(pos) = glm::vec2(0.0f);
  DEFINE_VERTEX_GET_INSTANCE_BINDING_DESCRIPTION(SpriteData);
};

static constexpr const auto kDefaultSpriteSheetMaxSize = 65536;
class SpriteSheet {
 private:
 public:
  SpriteSheet();
  ~SpriteSheet();
};
}  // namespace prt

#endif  // PRT_SPRITE_H
