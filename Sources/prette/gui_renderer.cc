#include "prette/gui_renderer.h"

#include <algorithm>
#include <exception>
#include <rx-subscription.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/device.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/framebuffer.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/gui.h"
#include "prette/image_view.h"  // IWYU pragma: keep
#include "prette/render_pass.h"
#include "prette/render_pass_builder.h"
#include "prette/render_target.h"
#include "prette/renderer.h"
#include "prette/rx.h"
#include "prette/sampler.h"
#include "prette/scene_renderer.h"
#include "prette/swapchain.h"
#include "prette/swapchain_event.h"
#include "prette/thread_local.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/vk_instance.h"  // IWYU pragma: keep
#include "prette/vk_physical_device.h"
#include "prette/window.h"

namespace prt {
static GuiRendererEventSubject events_{};

auto OnGuiRendererEvent() -> GuiRendererEventObservable {
  return events_.get_observable();
}

static inline void PublishEvent(GuiRendererEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

GuiRenderer::GuiRenderer(const VkRenderPassCreateInfo* create_info) :
  vk::RenderPass("gui", create_info) {
  InitImgui();
}

GuiRenderer::~GuiRenderer() {
  on_tick_.unsubscribe();
  delete sampler_;
  const auto driver = Driver::Get();
  std::ranges::for_each(framebuffers_, [this](vk::Framebuffer* framebuffer) {
    delete framebuffer;
  });
  vkFreeDescriptorSets(*driver->GetDevice(), driver->GetDescriptorPool(), descriptors_.size(), descriptors_.data());
}

void GuiRenderer::InitImgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  const auto window = GetAppWindow();
  ASSERT(window);
  ImGui_ImplGlfw_InitForVulkan(window->GetHandle(), true);
  auto& io = ImGui::GetIO();
  io.IniFilename = nullptr;

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

auto GuiRenderer::CreateSampler() -> vk::Sampler* {
  vk::SamplerBuilder builder{};
  return builder.WithLinearFilter()
      .WithRepeatAddressMode()
      .WithAnisotrophyDisabled()
      .WithMaxAnisotrophy(1.0f)
      .WithLinearMipmapMode()
      .Build();
}

auto GuiRenderer::New() -> GuiRenderer* {
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
                                                             .WithSource({
                                                                 .subpass = VK_SUBPASS_EXTERNAL,
                                                                 .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                             })
                                                             .WithDest({
                                                                 .subpass = 0,
                                                                 .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                                 .access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                             });
  return builder.BuildTyped<GuiRenderer>();
}

void GuiRenderer::OnSwapInit(const bool reinit) {
  try {
    const auto num_images = GetSwapchain()->GetNumberOfImages();
    InitFramebuffers();
    descriptors_.resize(num_images);
    sampler_ = CreateSampler();
    ASSERT(sampler_ && sampler_->IsInitialized());
    if (reinit) {
      for (const auto& descriptor : descriptors_) {
        ImGui_ImplVulkan_RemoveTexture(descriptor);
      }
    }

    for (auto idx = 0; idx < num_images; idx++) {
      const auto target = GetSceneRenderer()->GetTarget(idx);
      descriptors_[idx] =
          ImGui_ImplVulkan_AddTexture(*sampler_, *target->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    if (!reinit) {
      on_tick_ = OnTickEvent().subscribe([this](TickEvent* event) {
        ASSERT(event);
        const auto window = GetAppWindow();
        ASSERT(window);
        const auto size = window->GetSize();
        if (!gui::Update(size.data())) {
          // update more?
        }
      });
    }
  } catch (const std::exception& exc) {
    LOG(FATAL) << "exception: " << exc.what();
  }
}

void GuiRenderer::InitFramebuffers() {
  const auto swap = GetSwapchain();
  vk::FramebufferBuilder builder{};
  // clang-format off
  builder.WithSize(swap->GetExtent())
      .WithLayers(1)
      .WithRenderPass(this)
      .BuildWithAttachments(swap->GetViews(), framebuffers_);
  // clang-format on
}

auto GuiRenderer::GetCurrentSceneDescriptor() const -> VkDescriptorSet const& {
  const auto frame = GetSwapchain()->GetCurrentFrame();
  return GetSceneDescriptor(frame->GetFrame());
}

void GuiRenderer::OnSwapDestroyed(const bool is_reinit) {
  const auto driver = Driver::Get();
  for (const auto& framebuffer : framebuffers_) {
    delete framebuffer;
  }
  delete sampler_;
}

static const std::vector<VkClearValue> kClearValues = {
    {
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
};

void GuiRenderer::Execute() {
  const auto frame = GetSwapchain()->GetCurrentFrame();
  vk::CommandBufferScope buffer(GetCommandBuffer(frame->GetImage()));
  vk::RenderPassScope render_pass(buffer, this, *framebuffers_[frame->GetImage()]);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
}

static rx::subscription on_driver_init_{};
static rx::subscription on_destroying_driver_{};
static ThreadLocal<GuiRenderer> renderer_{};

void InitGuiRenderer() {
  ASSERT(!IsGuiRendererInitialized());
  on_driver_init_ = OnSwapchainCreatedEvent().subscribe([](SwapchainCreatedEvent* event) {
    renderer_ = GuiRenderer::New();
    on_driver_init_.unsubscribe();
  });
  on_destroying_driver_ = OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    ASSERT(event);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
  });
}

auto IsGuiRendererInitialized() -> bool {
  return renderer_.Get() != nullptr;
}

auto GetGuiRenderer() -> GuiRenderer* {
  ASSERT(IsGuiRendererInitialized());
  return renderer_;
}

}  // namespace prt