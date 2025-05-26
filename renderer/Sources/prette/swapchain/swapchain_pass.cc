#include "prette/swapchain/swapchain_pass.h"

#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_update.h"
#include "prette/gfx_vk.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/render_pass/render_pass.h"
#include "prette/render_pass/render_pass_builder.h"
#include "prette/surface.h"
#include "prette/swapchain/swapchain.h"
#include "prette/vk_cmd_buffers.h"

namespace prt::vk {
static const std::vector<VkClearValue> kDefaultClearValues = {
    VkClearValue{
        .color = {0.5f, 0.1f, 0.1f, 1.0f},
    },
};

auto SwapchainRenderPass::CreateDescriptors() -> vk::DescriptorSet* {
  DVLOG(2) << "creating swapchain descriptor set...";
  vk::DescriptorSetBuilder builder{};
  builder.WithName("swapchain");
  builder.AddUniformBufferBinding();
  return builder;
}

void SwapchainRenderPass::CreatePipelineLayout(vk::DescriptorSet* descriptors) {
  DVLOG(2) << "creating swapchain pipeline layout....";
  vk::PipelineLayoutBuilder builder{};
  // clang-format off
  builder.WithName("swapchain")
    .WithDescriptorSetLayouts(descriptors);
  // clang-format on
  vk::PipelineLayout* layout = builder;
  ASSERT_INITIALIZED(layout);
}

SwapchainRenderPass::SwapchainRenderPass(const std::string name, VkRenderPassCreateInfo* create_info) :
  RenderPass(std::move(name), create_info) {
  SetDescriptors(CreateDescriptors());
  CreatePipelineLayout(GetDescriptors());
  SetPipeline(Swapchain::CreatePipeline(GetDescriptors()));
}

void SwapchainRenderPass::UpdateDescriptors() {
  vk::DescriptorSetUpdate update(GetDescriptors());
  update.AddWriteCameraUniformBuffer(0, GetCamera());
}

void SwapchainRenderPass::Execute() {
  const auto swap = GetSwapchain();
  const auto frame = swap->GetCurrentFrame();
  vk::CommandBufferScope buffer(GetCommandBuffer(frame->GetImage()));
  vk::RenderPassScope render_pass(buffer, this, *frame->GetFramebuffer(), kDefaultClearValues);
  GetPipeline()->Bind(buffer);
  const auto extent = glm::to_vec2(GetSwapchain()->GetExtent());
  vk::SetViewport(buffer, extent);
  vk::SetScissor(buffer, extent);
}

auto SwapchainRenderPass::New() -> SwapchainRenderPass* {
  const auto driver = Driver::Get();
  vk::RenderPassBuilder builder{};
  builder.WithName("swapchain");
  const auto color_ref = builder.AddAttachment()
                             .WithFormat(driver->GetSurface()->GetImageFormat())
                             .WithSamples(VK_SAMPLE_COUNT_1_BIT)
                             .WithLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
                             .WithStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
                             .WithInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED)
                             .WithFinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                             .Build();

  // clang-format off
  vk::RenderPassBuilder::SubpassBuilder subpass_builder = builder.AddSubpass()
    .WithColorAttachment(color_ref);
  subpass_builder.BindGraphics();
  // clang-format on

  builder.AddSubpassDependency()
      .WithDependencyFlags(VK_DEPENDENCY_BY_REGION_BIT)
      .WithDest({
          .subpass = 0,
          .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          .access = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      })
      .WithSource({
          .subpass = VK_SUBPASS_EXTERNAL,
          .stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
          .access = VK_ACCESS_MEMORY_READ_BIT,
      });
  return builder.BuildNamedType<SwapchainRenderPass>();
}
}  // namespace prt::vk