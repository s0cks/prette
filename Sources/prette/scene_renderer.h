#ifndef PRT_SCENE_RENDERER_H
#define PRT_SCENE_RENDERER_H

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"

namespace prt {
class Driver;
class SceneRenderer {
 private:
  static void InitPipeline(const Driver* driver);
  static void InitFramebuffers(const Driver* driver);
  static void InitRenderPass(const Driver* driver);
  static void InitCommandPool(const Driver* driver);
  static void InitCommandBuffers(const Driver* driver);
  static void InitBuffers();

  static void Destroy(const Driver* driver, const bool is_reinit);

 public:
  static void Init();
  static auto GetRenderPass() -> VkRenderPass const&;
  static auto GetCommandPool() -> VkCommandPool const&;
  static void Draw(const uint32_t buffer_index, const uint32_t image_index, std::vector<VkCommandBuffer>& cmd_buffers);
};
}  // namespace prt

#endif  // PRT_SCENE_RENDERER_H
