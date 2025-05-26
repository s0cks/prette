#include "prette/swapchain/swapchain.h"

#include <algorithm>
#include <exception>
#include <string>
#include <sys/param.h>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/event.h"
#include "prette/framebuffer.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/image/image_view.h"
#include "prette/image/image_view_builder.h"
#include "prette/pipeline/pipeline.h"
#include "prette/render_pass/render_pass.h"
#include "prette/renderer.h"
#include "prette/rx.h"
#include "prette/surface.h"
#include "prette/swapchain/swapchain_builder.h"
#include "prette/swapchain/swapchain_event.h"
#include "prette/swapchain/swapchain_frame.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_physical_device.h"
#include "prette/vk_queue.h"

namespace prt {
DEFINE_GLOBAL_EVENT_SUBJECT(SwapchainEvent, events);

static rx::subscription on_driver_init_{};
static rx::subscription on_destroying_driver_{};
static ThreadLocal<Swapchain> swapchain_;

void Swapchain::InitImages(const VkSwapchainKHR& swapchain, std::vector<VkImage>& images) {
  const auto driver = Driver::Get();
  uint32_t num_images = 0;
  vkGetSwapchainImagesKHR(*driver->GetDevice(), swapchain, &num_images, nullptr);
  images.resize(num_images);
  vkGetSwapchainImagesKHR(*driver->GetDevice(), swapchain, &num_images, &images[0]);
}

Swapchain::Swapchain(const VkSwapchainCreateInfoKHR* create_info) :
  format_(create_info->imageFormat),
  extent_(create_info->imageExtent) {
  ASSERT(create_info);
  const auto driver = Driver::Get();
  driver->CreateSwapchain(create_info, handle_ptr());
  InitImages(handle_ref(), images_);
  {
    // init image views
    vk::ImageViewBuilder builder;
    // clang-format off
    builder.WithFormat(format_)
      .WithType(VK_IMAGE_VIEW_TYPE_2D)
      .WithIdentityComponents()
      .BuildAll(images_, views_);
    // clang-format on
  }

  {
    // init framebuffers
    vk::FramebufferBuilder builder{};
    // clang-format off
    builder.WithRenderPass(GetRenderer()->GetRenderPass())
      .WithSize(GetExtent())
      .WithLayers(1)
      .BuildWithAttachments(views_, framebuffers_);
    // clang-format on
  }
  try {
    frames_ = new SwapchainFrameRingBuffer(this, images_.size());
    ASSERT(frames_);
  } catch (std::exception exc) {
    LOG(FATAL) << "failed to create swapchain ring buffer: " << exc.what();
  }
}

Swapchain::~Swapchain() {
  const auto driver = Driver::Get();
  driver->DestroySwapchain(handle_ref());
  delete frames_;
  std::ranges::for_each(framebuffers_, [](vk::Framebuffer* framebuffer) {
    delete framebuffer;
  });
  std::ranges::for_each(views_, [](vk::ImageView* view) {
    delete view;
  });
}

auto Swapchain::ToString() const -> std::string {
  return ToStringHelper<Swapchain>{};
}

auto Swapchain::GetImage(const uint32_t idx) const -> const VkImage& {
  return images_[idx];
}

auto Swapchain::GetView(const uint32_t idx) const -> vk::ImageView* {
  return views_.at(idx);
}

auto Swapchain::GetFramebuffer(const uint32_t idx) const -> vk::Framebuffer* {
  return framebuffers_.at(idx);
}

static inline auto CreateSwapchain(vk::Surface* surface) -> Swapchain* {
  SwapchainBuilder builder{};
  builder.WithImageArrayLayers(1)
      .WithColorImageUsage()
      .WithSurface(surface)
      .WithCompositeAlpha(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
      .WithClippedEnabled();
  return builder.Build();
}

void SwapchainInitializer::operator()() {
  const auto driver = Driver::Get();
  if (IsReinit())
    driver->WaitDeviceIdle();

  if (IsSwapchainInitialized()) {
    ASSERT(IsReinit() || !init_);
    swapchain_.Release();
    Swapchain::Publish<SwapchainDestroyedEvent>(IsReinit());
  }

  if (init_) {
    swapchain_ = CreateSwapchain(driver->GetSurface());
    ASSERT(swapchain_ && swapchain_->IsInitialized());
    if (reinit_)
      GetSwapchain()->SetResized(false);

    if (!reinit_)
      Swapchain::Publish<SwapchainCreatedEvent>(IsReinit());
    Swapchain::Publish<SwapchainInitEvent>();
  }
}

auto IsSwapchainInitialized() -> bool {
  return swapchain_.Get() != nullptr;
}

auto GetSwapchain() -> Swapchain* {
  ASSERT(IsSwapchainInitialized());
  return swapchain_;
}

void Swapchain::PublishEvent(SwapchainEvent* event) {
  ASSERT(event);
  const auto& subscriber = events.get_subscriber();
  return subscriber.on_next(event);
}

auto Swapchain::CreatePipeline(vk::DescriptorSet* dset) -> vk::RenderPipeline* {
  return vk::RenderPipeline::FromJson("swapchain.json");
}
}  // namespace prt