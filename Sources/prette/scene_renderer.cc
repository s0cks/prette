#include "prette/scene_renderer.h"

#include <algorithm>
#include <fmt/format.h>
#include <string>
#include <tiny_obj_loader.h>
#include <vector>

#include "prette/camera.h"
#include "prette/chunk.h"
#include "prette/chunk_renderer.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/pipeline.h"
#include "prette/render_pass.h"
#include "prette/render_pass_builder.h"
#include "prette/render_target.h"
#include "prette/renderer.h"
#include "prette/rx.h"
#include "prette/swapchain.h"
#include "prette/swapchain_event.h"
#include "prette/texture.h"
#include "prette/thread_local.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/vk_physical_device.h"
#include "prette/world.h"
#include "prette/world_manager.h"

namespace prt {
auto SceneRenderer::CreatePipeline() -> vk::RenderPipeline* {
  return vk::RenderPipeline::FromJson("scene.json");
}

auto SceneRenderer::CreateDescriptorSet() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder;
  builder.WithName("scene");
  builder.AddUniformBufferBinding();
  return builder.Build();
}

SceneRenderer::SceneRenderer(const VkRenderPassCreateInfo* create_info) :
  RenderPassTemplate<RenderTarget>("scene", create_info) {
  SetDescriptors(CreateDescriptorSet());
  SetPipeline(CreatePipeline());
  chunk_renderer_ = new ChunkRenderer();  // NOLINT TODO: remove nolint
  ASSERT(chunk_renderer_);
}

SceneRenderer::~SceneRenderer() {
  delete depth_texture_;
  delete chunk_renderer_;
  on_swap_created_.unsubscribe();
  std::ranges::for_each(targets_, [](RenderTarget* target) {
    delete target;
  });
}

void SceneRenderer::InitDepthTexture() {
  const auto driver = Driver::Get();
  const auto& format = driver->GetDepthFormat();
  const auto& extent = GetSwapchain()->GetExtent();
  depth_texture_ = NewDepthTexture("scene-depth", extent, format);
  ASSERT_INITIALIZED(depth_texture_);
}

void SceneRenderer::UpdateDescriptors() {
  const auto camera = GetCamera();
  vk::DescriptorSetUpdate update(*GetDescriptors());
  // clang-format off
  update.AddWriteUniformBuffer(0)
    .WithBufferInfo(&camera->GetBuffer()->GetDescriptor());
  // clang-format on
}

void SceneRenderer::OnSwapInit(const bool reinit) {
  InitDepthTexture();
  for (auto idx = 0; idx < GetSwapchain()->GetNumberOfImages(); idx++) {
    const auto target = new RenderTarget(idx, this, GetSwapchain()->GetExtent(), kDefaultRenderTargetFormat,
                                         GetDepthTexture()->GetImageView());
    ASSERT_INITIALIZED(target);
    targets_.at(idx) = target;
  }
  if (!reinit) {
    UpdateDescriptors();
  }
}

void SceneRenderer::OnSwapDestroyed(const bool reinit) {
  delete depth_texture_;
  if (!reinit)
    delete chunk_renderer_;
  std::ranges::for_each(targets_, [](RenderTarget* target) {
    delete target;
  });
}

static const std::vector<VkClearValue> kClearValues = {
    VkClearValue{
        .color = {0.3f, 0.3f, 0.3f, 1.0f},
    },
    VkClearValue{
        .depthStencil = {1.0f, 0.0f},
    },
};

void SceneRenderer::Execute() {
  const auto frame = GetSwapchain()->GetCurrentFrame();
  vk::CommandBufferScope buffer(GetCommandBuffer(frame->GetFrame()), true);
  vk::RenderPassScope render_pass(buffer, this, *GetTargetFramebuffer(frame->GetFrame()), kClearValues);
  // chunk
  ASSERT(IsWorldInitialized());
  const auto chunk = GetWorld()->GetChunkAt(ChunkPos(0, 0));
  GetChunkRenderer()->Render(buffer, chunk);
}

auto SceneRenderer::New() -> SceneRenderer* {
  vk::RenderPassBuilder builder{};
  builder.WithName("scene");
  auto color_ref = builder.AddAttachment()
                       .WithFormat(GetSwapchain()->GetFormat())
                       .WithLoadOpClear()
                       .WithStoreOpStore()
                       .WithInitialLayoutUndefined()
                       .WithFinalLayoutShaderReadOptimal()
                       .BuildWithColorAttachmentOptimalRef();

  auto depth_ref = builder.AddAttachment()
                       .WithFormat(Driver::Get()->GetDepthFormat())
                       .WithLoadOpClear()
                       .WithStoreOpStore()
                       .WithInitialLayoutUndefined()
                       .WithFinalLayoutDepthStecilOptimal()
                       .Build();

  // clang-format off
  vk::RenderPassBuilder::SubpassBuilder subpass_builder = builder.AddSubpass()
    .WithColorAttachment(color_ref)
    .WithDepthAttachment(depth_ref);
  subpass_builder.BindGraphics();
  // clang-format on
  builder.AddSubpassDependency()
      .WithDependencyByRegion()
      .WithSource({
          .subpass = VK_SUBPASS_EXTERNAL,
          .stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
          .access = VK_ACCESS_MEMORY_READ_BIT,
      })
      .WithDest({
          .subpass = 0,
          .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
          .access = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      });
  return builder.BuildTyped<SceneRenderer>();
}

static rx::subscription on_renderer_init_{};
static rx::subscription on_renderer_destroyed_{};
static ThreadLocal<SceneRenderer> renderer_{};

void InitSceneRenderer() {
  ASSERT(!IsSceneRendererInitialized());
  on_renderer_init_ = OnSwapchainCreatedEvent().subscribe([](SwapchainCreatedEvent* event) {
    renderer_ = SceneRenderer::New();
    ASSERT(IsSceneRendererInitialized());
    on_renderer_init_.unsubscribe();
  });
}

auto IsSceneRendererInitialized() -> bool {
  return renderer_.Get() != nullptr;
}

auto GetSceneRenderer() -> SceneRenderer* {
  ASSERT(IsSceneRendererInitialized());
  return renderer_.Get();
}
}  // namespace prt