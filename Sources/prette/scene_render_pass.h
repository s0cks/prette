#ifndef PRT_SCENE_RENDER_PASS_H
#define PRT_SCENE_RENDER_PASS_H

#include "prette/gfx.h"

namespace prt {
class SceneRenderPass {
 private:
  VkRenderPass pass_{};

 public:
  SceneRenderPass();
  ~SceneRenderPass();

  auto Get() const -> const VkRenderPass& {
    return pass_;
  }
};
}  // namespace prt

#endif  // PRT_SCENE_RENDER_PASS_H
