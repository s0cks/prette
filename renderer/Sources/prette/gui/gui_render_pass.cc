#include "prette/gui/gui_render_pass.h"

#include <algorithm>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"
#include "prette/framebuffer.h"
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/gui/gui.h"
#include "prette/image/image_view.h"  // IWYU pragma: keep
#include "prette/render_pass/render_pass.h"
#include "prette/render_pass/render_pass_builder.h"
#include "prette/render_pass/scene_renderer.h"
#include "prette/renderer.h"
#include "prette/rx.h"
#include "prette/swapchain/swapchain.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/vk_instance.h"  // IWYU pragma: keep
#include "prette/vk_physical_device.h"

namespace prt {
static GuiRenderPassEventSubject events_{};

auto OnGuiRenderPassEvent() -> GuiRenderPassEventObservable {
  return events_.get_observable();
}

static inline void PublishEvent(GuiRenderPassEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

GuiRenderPass::GuiRenderPass(const VkRenderPassCreateInfo* create_info) :
  vk::RenderPass("gui", create_info) {
  InitImgui();
  OnSwapInit(false);
}

GuiRenderPass::~GuiRenderPass() {
  const auto driver = Driver::Get();
  std::ranges::for_each(framebuffers_, [this](vk::Framebuffer* framebuffer) {
    delete framebuffer;
  });
}

void GuiRenderPass::InitImgui() {
  const auto driver = Driver::Get();
  ImGui_ImplVulkan_InitInfo info{};
  info.Instance = *driver->GetInstance();
  info.PhysicalDevice = *driver->GetPhysicalDevice();
  info.Allocator = driver->GetAllocator();
  info.Device = *driver->GetDevice();
  info.Queue = driver->GetDevice()->GetGraphicsQueue();
  info.DescriptorPool = driver->GetDescriptorPool();
  info.ImageCount = MAX_NUMBER_OF_FRAMES_IN_FLIGHT;
  info.MinImageCount = MAX_NUMBER_OF_FRAMES_IN_FLIGHT;
  info.RenderPass = GetHandle();
  info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  ImGui_ImplVulkan_Init(&info);

  ImGui_ImplVulkan_CreateFontsTexture();
  ImGui_ImplVulkan_DestroyFontsTexture();
}

void GuiRenderPass::OnSwapInit(const bool reinit) {
  InitFramebuffers();
}

void GuiRenderPass::OnSwapDestroyed(const bool is_reinit) {
  std::ranges::for_each(framebuffers_, [](vk::Framebuffer* framebuffer) {
    delete framebuffer;
  });
}

void GuiRenderPass::InitFramebuffers() {
  const auto swap = GetSwapchain();
  vk::FramebufferBuilder builder{};
  // clang-format off
  builder.WithSize(swap->GetExtent())
      .WithLayers(1)
      .WithRenderPass(this)
      .BuildWithAttachments(swap->GetViews(), framebuffers_);
  // clang-format on
}

void GuiRenderPass::Execute() {
  const auto frame = GetSwapchain()->GetCurrentFrame();
  vk::CommandBufferScope buffer(GetCommandBuffer(frame->GetImage()));
  vk::RenderPassScope render_pass(buffer, this, *GetSwapchain()->GetFramebuffer(frame->GetImage()));
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
}

auto GuiRenderPass::New() -> GuiRenderPass* {
  vk::RenderPassBuilder builder{};
  const auto color_ref = builder.AddAttachment()
                             .WithFormat(GetSwapchain()->GetFormat())
                             .WithSamples(VK_SAMPLE_COUNT_1_BIT)
                             .WithLoadOp(VK_ATTACHMENT_LOAD_OP_LOAD)
                             .WithStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
                             .WithInitialLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                             .WithFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
                             .Build();

  // clang-format off
  auto subpass = builder.AddSubpass()
    .WithColorAttachment(color_ref);
  subpass.BindGraphics();
  // clang-format on

  vk::RenderPassBuilder::SubpassDepBuilder subpass_dep = builder.AddSubpassDependency()
                                                             .WithDependencyByRegion()
                                                             .WithSource({
                                                                 .subpass = VK_SUBPASS_EXTERNAL,
                                                                 .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                             })
                                                             .WithDest({
                                                                 .subpass = 0,
                                                                 .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                                 .access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                             });
  return builder.BuildTyped<GuiRenderPass>();
}
}  // namespace prt