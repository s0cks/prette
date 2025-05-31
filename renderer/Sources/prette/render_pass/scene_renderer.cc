#include "prette/render_pass/scene_renderer.h"

#include <algorithm>
#include <fmt/format.h>
#include <string>
#include <tiny_obj_loader.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/chunk_renderer.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/framebuffer.h"
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/gui/gui.h"
#include "prette/pipeline/pipeline.h"
#include "prette/render_pass/render_pass.h"
#include "prette/render_pass/render_pass_builder.h"
#include "prette/render_pass/render_target.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/rx.h"
#include "prette/sampler.h"
#include "prette/surface.h"
#include "prette/swapchain/swapchain.h"
#include "prette/swapchain/swapchain_event.h"
#include "prette/texture.h"
#include "prette/thread_local.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/vk_physical_device.h"
#include "prette/world/world.h"
#include "prette/world/world_manager.h"

namespace prt {
SceneRenderPass::SceneRenderPass(const VkRenderPassCreateInfo* create_info) :
  RenderPassTemplate<RenderTarget>("scene", create_info) {
  OnSwapInit(false);
}

SceneRenderPass::~SceneRenderPass() {
  std::ranges::for_each(targets_, [](RenderTarget* target) {
    delete target;
  });
}

void SceneRenderPass::OnSwapInit(const bool reinit) {
  RenderPass::OnSwapInit(reinit);
  InitFramebuffers();
  for (auto idx = 0; idx < GetSwapchain()->GetNumberOfImages(); idx++) {
    const auto target = new RenderTarget(idx, this, GetSwapchain()->GetExtent(), kDefaultRenderTargetFormat);
    ASSERT_INITIALIZED(target);
    targets_.at(idx) = target;
  }
}

void SceneRenderPass::OnSwapDestroyed(const bool reinit) {
  RenderPass::OnSwapDestroyed(reinit);
  std::ranges::for_each(targets_, [](RenderTarget* target) {
    delete target;
  });
}

auto SceneRenderPass::New() -> SceneRenderPass* {
  vk::RenderPassBuilder builder{};
  builder.WithName("scene");
  const auto driver = Driver::Get();
  auto color_ref = builder.AddAttachment()
                       .WithFormat(GetSwapchain()->GetFormat())
                       .WithLoadOpLoad()
                       .WithStoreOpStore()
                       .WithInitialLayoutUndefined()
                       .WithFinalLayoutColorAttachmentOptimal()
                       .Build();

  // auto depth_ref = builder.AddAttachment()
  //                      .WithFormat(Driver::Get()->GetDepthFormat())
  //                      .WithLoadOpClear()
  //                      .WithStoreOpStore()
  //                      .WithInitialLayoutUndefined()
  //                      .WithFinalLayoutDepthStecilOptimal()
  //                      .Build();

  // clang-format off
  vk::RenderPassBuilder::SubpassBuilder subpass_builder = builder.AddSubpass()
    .WithColorAttachment(color_ref);
  subpass_builder.BindGraphics();
  // clang-format on
  builder.AddSubpassDependency()
      .WithDependencyByRegion()
      .WithSource({
          .subpass = VK_SUBPASS_EXTERNAL,
          .stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
          .access = VK_ACCESS_MEMORY_READ_BIT,
      })
      .WithDest({
          .subpass = 0,
          .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          .access = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      });
  return builder.BuildTyped<SceneRenderPass>();
}

auto SceneRenderer::CreatePipeline() -> vk::RenderPipeline* {
  return vk::RenderPipeline::FromJson("scene.json");
}

auto SceneRenderer::CreateDescriptorSet() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder;
  builder.WithName("scene");
  builder.AddUniformBufferBinding();
  return builder.Build();
}

void SceneRenderPass::InitFramebuffers() {
  const auto swap = GetSwapchain();
  vk::FramebufferBuilder builder{};
  // clang-format off
  builder.WithSize(swap->GetExtent())
      .WithLayers(1)
      .WithRenderPass(this)
      .BuildWithAttachments(swap->GetViews(), framebuffers_);
  // clang-format on
}

SceneRenderer::SceneRenderer() {
  OnSwapchainCreated([this](SwapchainCreatedEvent* event) {
    InitDepthTexture();
    pass_ = SceneRenderPass::New();
    ASSERT_INITIALIZED(pass_);
  });
  OnSwapchainInit([this](SwapchainInitEvent* event) {
    sampler_ = CreateSampler();
    ASSERT(sampler_ && sampler_->IsInitialized());
    scene_descriptors_.resize(pass_->GetNumberOfTargets());
    for (auto idx = 0; idx < pass_->GetNumberOfTargets(); idx++) {
      const auto target = pass_->GetTarget(idx);
      GetSceneRenderer()->scene_descriptors_[idx] =
          ImGui_ImplVulkan_AddTexture(*GetSampler(), *target->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
  });
  OnSwapchainDestroyed([this](SwapchainDestroyedEvent* event) {
    delete depth_texture_;
    for (const auto& descriptor : scene_descriptors_) {
      ImGui_ImplVulkan_RemoveTexture(descriptor);
    }
  });
  OnInitDescriptorSets([this](InitDescriptorSetsEvent* event) {
    descriptors_ = CreateDescriptorSet();
    ASSERT_INITIALIZED(descriptors_);
  });
  OnInitGraphicsPipelines([this](InitGraphicsPipelinesEvent* event) {
    pipeline_ = CreatePipeline();
    ASSERT_INITIALIZED(pipeline_);
  });
  OnInitBuffers([this](InitBuffersEvent* event) {
    UpdateDescriptors();
  });
  // colored_quads_ = new color2d::SingleQuadPipeline();
  // ASSERT_INITIALIZED(colored_quads_);
}

SceneRenderer::~SceneRenderer() {
  delete depth_texture_;
  delete sampler_;
  delete depth_texture_;
  on_swap_created_.unsubscribe();
  const auto driver = Driver::Get();
  vkFreeDescriptorSets(*driver->GetDevice(), driver->GetDescriptorPool(), scene_descriptors_.size(),
                       scene_descriptors_.data());
  delete descriptors_;
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
  vk::DescriptorSetUpdate update(descriptors_);
  // clang-format off
  update.AddWriteUniformBuffer(0)
    .WithBufferInfo(&camera->GetBuffer()->GetDescriptor());
  // clang-format on
}

auto SceneRenderer::CreateSampler() -> vk::Sampler* {
  vk::SamplerBuilder builder{};
  return builder.WithLinearFilter()
      .WithRepeatAddressMode()
      .WithAnisotrophyDisabled()
      .WithMaxAnisotrophy(1.0f)
      .WithLinearMipmapMode()
      .Build();
}

auto SceneRenderer::GetCurrentSceneDescriptor() const -> VkDescriptorSet {
  const auto frame = GetSwapchain()->GetCurrentFrame();
  return GetSceneDescriptor(frame->GetFrame());
}

static const std::vector<VkClearValue> kClearValues = {
    VkClearValue{
        .color = {0.3f, 0.3f, 0.3f, 1.0f},
    },
    VkClearValue{
        .depthStencil = {1.0f, 0.0f},
    },
};

void SceneRenderPass::Execute() {
  ASSERT(GetSceneRenderer()->IsInitialized());
  const auto frame = GetSwapchain()->GetCurrentFrame();
  vk::CommandBufferScope buffer(GetCommandBuffer(frame->GetImage()), true);
  vk::RenderPassScope render_pass(buffer, this, *GetSwapchain()->GetFramebuffer(frame->GetImage()), kClearValues);
  ASSERT(IsWorldInitialized());
  const auto chunk = GetWorld()->GetChunkAt(ChunkPos(0, 0));
  GetSceneRenderer()->chunk_renderer_.Render(buffer, chunk);
  GetSceneRenderer()->sprites_.Render(buffer);
}

static rx::subscription on_renderer_init_{};
static rx::subscription on_renderer_destroyed_{};
static ThreadLocal<SceneRenderer> renderer_{};

void InitSceneRenderer() {
  ASSERT(!IsSceneRendererInitialized());
  renderer_ = new SceneRenderer();
  ASSERT(IsSceneRendererInitialized());
}

auto IsSceneRendererInitialized() -> bool {
  return renderer_.Get() != nullptr;
}

auto GetSceneRenderer() -> SceneRenderer* {
  ASSERT(IsSceneRendererInitialized());
  return renderer_.Get();
}
}  // namespace prt