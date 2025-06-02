#ifndef PRT_SCENE_RENDERER_H
#define PRT_SCENE_RENDERER_H

#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/chunk_renderer.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_layout.h"
#include "prette/pipeline/pipeline.h"
#include "prette/render_pass/render_pass.h"
#include "prette/render_pass/render_target.h"
#include "prette/rx.h"
#include "prette/sampler.h"
#include "prette/sprite.h"
#include "prette/texture.h"
#include "prette/vk.h"

namespace prt {
class Driver;
class RenderTarget;
class ChunkRenderer;
class SwapchainFrame;
namespace vk {
class RenderPassBuilder;
}

class SceneRenderPass : public vk::RenderPassTemplate<RenderTarget> {
 private:
  std::vector<vk::Framebuffer*> framebuffers_{};

  void InitFramebuffers();
  void OnSwapInit(const bool reinit) override;
  void OnSwapDestroyed(const bool reinit) override;

 public:
  explicit SceneRenderPass(const VkRenderPassCreateInfo* create_info);
  ~SceneRenderPass() override;
  void Execute() override;

 public:
  static auto New() -> SceneRenderPass*;
};

class SceneRenderer {
  friend class SceneRenderPass;
  friend class vk::RenderPassBuilder;
  static auto CreatePipeline() -> vk::RenderPipeline*;
  static auto CreateDescriptorSet() -> vk::DescriptorSet*;
  static auto CreateSampler() -> vk::Sampler*;

 private:
  Texture* depth_texture_ = nullptr;
  SceneRenderPass* pass_ = nullptr;
  vk::Sampler* sampler_ = nullptr;
  std::vector<VkDescriptorSet> scene_descriptors_{};
  vk::DescriptorSet* descriptors_ = nullptr;
  vk::RenderPipeline* pipeline_ = nullptr;
  ChunkRenderer chunk_renderer_{};
  // SpriteRenderer sprites_{};
  rx::subscription on_swap_created_{};

  void InitDepthTexture();
  void UpdateDescriptors();
  static void InitBuffers();

  auto GetChunkRenderer() const -> const ChunkRenderer& {
    return chunk_renderer_;
  }

 public:
  SceneRenderer();
  ~SceneRenderer();

  auto GetScenePass() const -> SceneRenderPass* {
    return pass_;
  }

  auto GetDepthTexture() const -> Texture* {
    return depth_texture_;
  }

  auto GetSampler() const -> vk::Sampler* {
    return sampler_;
  }

  auto GetSceneDescriptor(const uint32_t frame) const -> VkDescriptorSet {
    if (scene_descriptors_.empty())
      return VK_NULL_HANDLE;
    return scene_descriptors_[frame];
  }

  auto GetTileDescriptorSetLayout() const -> vk::DescriptorSetLayout* {
    return chunk_renderer_.GetTileDescriptorSetLayout();
  }

  auto IsInitialized() const -> bool {
    return chunk_renderer_.IsInitialized();
  }

  auto GetCurrentSceneDescriptor() const -> VkDescriptorSet;

 public:
  static auto New() -> SceneRenderer*;
};

void InitSceneRenderer();
auto IsSceneRendererInitialized() -> bool;
auto GetSceneRenderer() -> SceneRenderer*;
}  // namespace prt

#endif  // PRT_SCENE_RENDERER_H
