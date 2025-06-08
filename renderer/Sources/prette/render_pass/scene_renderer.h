#ifndef PRT_SCENE_RENDERER_H
#define PRT_SCENE_RENDERER_H

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/chunk_renderer.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_layout.h"
#include "prette/framebuffer/framebuffer_attachment.h"
#include "prette/gbuffer.h"
#include "prette/pipeline/pipeline.h"
#include "prette/render_pass/render_pass.h"
#include "prette/rx.h"
#include "prette/sampler.h"
#include "prette/sprite.h"
#include "prette/vk.h"

namespace prt {
class Driver;
class RenderTarget;
class ChunkRenderer;
class SwapchainFrame;
namespace vk {
class RenderPassBuilder;
}  // namespace vk

class SceneRenderPass : public vk::RenderPass {
 private:
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
  SceneRenderPass* pass_ = nullptr;
  vk::Sampler* sampler_ = nullptr;
  vk::DescriptorSet* descriptors_ = nullptr;
  vk::RenderPipeline* pipeline_ = nullptr;
  ChunkRenderer chunk_renderer_{};
  rx::subscription on_swap_created_{};
  rx::subscription on_swap_init_{};
  GBuffer::ColorAttachmentArray color_attachments_{};
  Framebuffer* framebuffer_ = nullptr;
  FramebufferAttachment* depth_attachment_ = nullptr;

  void UpdateDescriptors();
  static void InitBuffers();
  void InitColorAttachments(const VkExtent2D& extent);
  void InitDepthAttachment(const VkExtent2D& extent);
  void InitFramebuffer(const VkExtent2D& extent);

  auto GetChunkRenderer() const -> const ChunkRenderer& {
    return chunk_renderer_;
  }

 public:
  SceneRenderer();
  ~SceneRenderer();

  auto GetScenePass() const -> SceneRenderPass* {
    return pass_;
  }

  auto GetSampler() const -> vk::Sampler* {
    return sampler_;
  }

  auto GetTileDescriptorSetLayout() const -> vk::DescriptorSetLayout* {
    return chunk_renderer_.GetTileDescriptorSetLayout();
  }

  auto GetColorAttachment(const uint64_t idx) const -> FramebufferAttachment* {
    return color_attachments_.at(idx);
  }

  auto GetDepthAttachment() const -> FramebufferAttachment* {
    return depth_attachment_;
  }

  auto GetFramebuffer() const -> Framebuffer* {
    return framebuffer_;
  }

  auto IsInitialized() const -> bool {
    return chunk_renderer_.IsInitialized();
  }

 public:
  static auto New() -> SceneRenderer*;
};

void InitSceneRenderer();
auto IsSceneRendererInitialized() -> bool;
auto GetSceneRenderer() -> SceneRenderer*;
}  // namespace prt

#endif  // PRT_SCENE_RENDERER_H
