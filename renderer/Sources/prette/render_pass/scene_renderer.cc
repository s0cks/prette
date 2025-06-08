#include "prette/render_pass/scene_renderer.h"

#include <fmt/format.h>
#include <string>
#include <tiny_obj_loader.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk_renderer.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/framebuffer/framebuffer_attachment.h"
#include "prette/framebuffer/framebuffer_attachment_builder.h"
#include "prette/framebuffer/framebuffer_builder.h"
#include "prette/gbuffer.h"
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/pipeline/pipeline.h"
#include "prette/render_pass/render_pass.h"
#include "prette/render_pass/render_pass_builder.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/rx.h"
#include "prette/sampler.h"
#include "prette/surface.h"
#include "prette/swapchain/swapchain.h"
#include "prette/swapchain/swapchain_event.h"
#include "prette/thread_local.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/vk_physical_device.h"
#include "prette/world/world_manager.h"

namespace prt {
SceneRenderPass::SceneRenderPass(const VkRenderPassCreateInfo* create_info) :
  RenderPass("scene", create_info) {
  OnSwapInit(false);
}

SceneRenderPass::~SceneRenderPass() {
  OnSwapDestroyed(false);
}

void SceneRenderPass::OnSwapInit(const bool reinit) {
  RenderPass::OnSwapInit(reinit);
}

void SceneRenderPass::OnSwapDestroyed(const bool reinit) {
  RenderPass::OnSwapDestroyed(reinit);
}

auto SceneRenderPass::New() -> SceneRenderPass* {
  vk::RenderPassBuilder builder{};
  builder.WithName("scene");
  const auto driver = Driver::Get();
  auto pos_ref = builder.AddAttachment()
                     .WithFormat(GBuffer::kPosFormat)
                     .WithLoadOpClear()
                     .WithStoreOpStore()
                     .WithInitialLayoutUndefined()
                     .WithFinalLayoutShaderReadOptimal()
                     .BuildWithColorAttachmentOptimalRef();
  auto normal_ref = builder.AddAttachment()
                        .WithFormat(GBuffer::kNormalFormat)
                        .WithLoadOpClear()
                        .WithStoreOpStore()
                        .WithInitialLayoutUndefined()
                        .WithFinalLayoutShaderReadOptimal()
                        .BuildWithColorAttachmentOptimalRef();
  auto albedo_ref = builder.AddAttachment()
                        .WithFormat(GBuffer::kAlbedoFormat)
                        .WithLoadOpClear()
                        .WithStoreOpStore()
                        .WithInitialLayoutUndefined()
                        .WithFinalLayoutShaderReadOptimal()
                        .BuildWithColorAttachmentOptimalRef();
  auto metallic_ref = builder.AddAttachment()
                          .WithFormat(GBuffer::kMetallicFormat)
                          .WithLoadOpClear()
                          .WithStoreOpStore()
                          .WithInitialLayoutUndefined()
                          .WithFinalLayoutShaderReadOptimal()
                          .BuildWithColorAttachmentOptimalRef();
  auto roughness_ref = builder.AddAttachment()
                           .WithFormat(GBuffer::kRoughnessFormat)
                           .WithLoadOpClear()
                           .WithStoreOpStore()
                           .WithInitialLayoutUndefined()
                           .WithFinalLayoutShaderReadOptimal()
                           .BuildWithColorAttachmentOptimalRef();
  auto ao_ref = builder.AddAttachment()
                    .WithFormat(GBuffer::kAoFormat)
                    .WithLoadOpClear()
                    .WithStoreOpStore()
                    .WithInitialLayoutUndefined()
                    .WithFinalLayoutShaderReadOptimal()
                    .BuildWithColorAttachmentOptimalRef();
  auto depth_ref = builder.AddAttachment()
                       .WithFormat(driver->GetDepthFormat())
                       .WithLoadOpClear()
                       .WithStoreOpStore()
                       .WithInitialLayoutUndefined()
                       .WithFinalLayoutShaderReadOptimal()
                       .BuildWithDepthStencilOptimalRef();

  // clang-format off
  vk::RenderPassBuilder::SubpassBuilder subpass_builder = builder.AddSubpass()
    .WithColorAttachment(pos_ref)
    .WithColorAttachment(normal_ref)
    .WithColorAttachment(albedo_ref)
    .WithColorAttachment(metallic_ref)
    .WithColorAttachment(roughness_ref)
    .WithColorAttachment(ao_ref)
    .WithDepthAttachment(depth_ref);
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
  builder.AddSubpassDependency()
      .WithDependencyByRegion()
      .WithSource({
          .subpass = 0,
          .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          .access = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      })
      .WithDest({
          .subpass = VK_SUBPASS_EXTERNAL,
          .stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
          .access = VK_ACCESS_MEMORY_READ_BIT,
      });
  return builder.BuildTyped<SceneRenderPass>();
}

void SceneRenderer::InitFramebuffer(const VkExtent2D& extent) {
  const auto swap = GetSwapchain();
  FramebufferBuilder builder{};
  framebuffer_ = builder.WithSize(extent)
                     .WithLayers(1)
                     .WithRenderPass(GetScenePass())
                     .WithAttachment(GetSceneRenderer()->GetColorAttachment(0)->GetImageView())  // pos
                     .WithAttachment(GetSceneRenderer()->GetColorAttachment(1)->GetImageView())  // normals
                     .WithAttachment(GetSceneRenderer()->GetColorAttachment(2)->GetImageView())  // albedo
                     .WithAttachment(GetSceneRenderer()->GetColorAttachment(3)->GetImageView())  // metallic
                     .WithAttachment(GetSceneRenderer()->GetColorAttachment(4)->GetImageView())  // roughness
                     .WithAttachment(GetSceneRenderer()->GetColorAttachment(5)->GetImageView())  // ao
                     .WithAttachment(GetSceneRenderer()->GetDepthAttachment()->GetImageView())   // depth
                     .Build();
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

SceneRenderer::SceneRenderer() {
  OnSwapchainCreated([this](SwapchainCreatedEvent* event) {
    const auto swap = GetSwapchain();
    const auto& extent = swap->GetExtent();
    InitColorAttachments(extent);
    InitDepthAttachment(extent);
    pass_ = SceneRenderPass::New();
    ASSERT_INITIALIZED(pass_);
    InitFramebuffer(extent);
  });
  OnSwapchainInit([this](SwapchainInitEvent* event) {
    sampler_ = CreateSampler();
    ASSERT(sampler_ && sampler_->IsInitialized());
  });
  OnSwapchainDestroyed([this](SwapchainDestroyedEvent* event) {

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
}

SceneRenderer::~SceneRenderer() {
  delete sampler_;
  on_swap_created_.unsubscribe();
  on_swap_init_.unsubscribe();
  const auto driver = Driver::Get();
  delete descriptors_;
}

void SceneRenderer::InitColorAttachments(const VkExtent2D& extent) {
  {
    FramebufferAttachmentBuilder builder(GBuffer::kPosFormat);
    // clang-format off
    color_attachments_[GBuffer::kPosition] = builder.WithExtent(extent)
      .WithUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .Build();
    // clang-format on
  }
  {
    FramebufferAttachmentBuilder builder(GBuffer::kNormalFormat);
    // clang-format off
    color_attachments_[GBuffer::kNormal] = builder.WithExtent(extent)
      .WithUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .Build();
    // clang-format on
  }
  {
    FramebufferAttachmentBuilder builder(GBuffer::kAlbedoFormat);
    // clang-format off
    color_attachments_[GBuffer::kAlbedo] = builder.WithExtent(extent)
      .WithUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .Build();
    // clang-format on
  }
  {
    FramebufferAttachmentBuilder builder(GBuffer::kMetallicFormat);
    // clang-format off
    color_attachments_[GBuffer::kMetallic] = builder.WithExtent(extent)
      .WithUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .Build();
    // clang-format on
  }
  {
    FramebufferAttachmentBuilder builder(GBuffer::kRoughnessFormat);
    // clang-format off
    color_attachments_[GBuffer::kRoughness] = builder.WithExtent(extent)
      .WithUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .Build();
    // clang-format on
  }
  {
    FramebufferAttachmentBuilder builder(GBuffer::kAoFormat);
    // clang-format off
    color_attachments_[GBuffer::kAo] = builder.WithExtent(extent)
      .WithUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
      .Build();
    // clang-format on
  }
}

void SceneRenderer::InitDepthAttachment(const VkExtent2D& extent) {
  const auto driver = Driver::Get();
  FramebufferAttachmentBuilder builder(driver->GetDepthFormat());
  // clang-format off
  depth_attachment_ = builder.WithExtent(extent)
    .WithUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    .Build();
  // clang-format on
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

static const std::vector<VkClearValue> kClearValues = {
    VkClearValue{
        .color = {0.3f, 0.3f, 0.3f, 1.0f},
    },
    VkClearValue{
        .color = {0.3f, 0.3f, 0.3f, 1.0f},
    },
    VkClearValue{
        .color = {0.3f, 0.3f, 0.3f, 1.0f},
    },
    VkClearValue{
        .color = {0.3f, 0.3f, 0.3f, 1.0f},
    },
    VkClearValue{
        .color = {0.3f, 0.3f, 0.3f, 1.0f},
    },
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
  vk::RenderPassScope render_pass(buffer, this, *GetSceneRenderer()->GetFramebuffer(), kClearValues);
  ASSERT(IsWorldInitialized());
  GetSceneRenderer()->chunk_renderer_.RenderChunks(buffer);
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