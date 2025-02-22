#ifndef PRT_SCENE_RENDERER_H
#define PRT_SCENE_RENDERER_H

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"
#include "prette/swapchain.h"

namespace prt {
class Driver;
class SceneRenderer {
 private:
  static void InitImages(const Driver* driver, const uint64_t num_images, const VkExtent2D& extent);
  static void InitDescriptorSetLayout(const Driver* driver);
  static void InitPipeline(const Driver* driver);
  static void InitPipelineLayout(const Driver* driver);
  static void InitRenderPass(const Driver* driver);
  static void InitCommandBuffers(const Driver* driver);
  static void InitBuffers();
  static void InitDescriptorSets(const Driver* driver);
  static void InitModel();
  static void InitModelTexture();
  static void InitDepthTexture(const Driver* driver);

  static void Destroy(const Driver* driver, const bool is_reinit);

 public:
  static void Init();
  static auto GetCameraBuffer(const uint64_t idx) -> vk::Buffer*;
  static auto GetRenderPass() -> VkRenderPass const&;
  static auto GetImageView(const uint64_t idx) -> VkImageView const&;
  static void Draw(const SwapChainFrame& frame, std::vector<VkCommandBuffer>& buffers);
};
}  // namespace prt

#endif  // PRT_SCENE_RENDERER_H
