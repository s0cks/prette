#ifndef PRT_SCENE_RENDERER_H
#define PRT_SCENE_RENDERER_H

#include "prette/descriptor_set.h"
#include "prette/pipeline.h"
#include "prette/render_pass.h"
#include "prette/render_target.h"
#include "prette/rx.h"
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

class SceneRenderer : public vk::RenderPassTemplate<RenderTarget> {
  friend class vk::RenderPassBuilder;
  static auto CreatePipeline() -> vk::RenderPipeline*;
  static auto CreateDescriptorSet() -> vk::DescriptorSet*;

 private:
  Texture* depth_texture_ = nullptr;
  ChunkRenderer* chunk_renderer_ = nullptr;
  rx::subscription on_swap_created_{};

  SceneRenderer(const VkRenderPassCreateInfo* create_info);

  void InitDepthTexture();
  void UpdateDescriptors();
  static void InitBuffers();

  auto GetChunkRenderer() const -> ChunkRenderer* {
    return chunk_renderer_;
  }

  void OnSwapInit(const bool reinit) override;
  void OnSwapDestroyed(const bool reinit) override;

 public:
  ~SceneRenderer() override;

  auto GetDepthTexture() const -> Texture* {
    return depth_texture_;
  }

  void Execute() override;

 public:
  static auto New() -> SceneRenderer*;
};

void InitSceneRenderer();
auto IsSceneRendererInitialized() -> bool;
auto GetSceneRenderer() -> SceneRenderer*;
}  // namespace prt

#endif  // PRT_SCENE_RENDERER_H
