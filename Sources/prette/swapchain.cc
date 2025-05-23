#include "prette/swapchain.h"

#include <algorithm>
#include <exception>
#include <string>
#include <sys/param.h>
#include <vector>

#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/framebuffer.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/image_view.h"
#include "prette/image_view_builder.h"
#include "prette/pipeline.h"
#include "prette/render_pass.h"
#include "prette/render_pass_builder.h"
#include "prette/renderer.h"
#include "prette/rx.h"
#include "prette/surface.h"
#include "prette/swapchain_builder.h"
#include "prette/swapchain_event.h"
#include "prette/swapchain_frame.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_physical_device.h"
#include "prette/vk_queue.h"

namespace prt {
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

static rx::subscription on_driver_init_{};
static rx::subscription on_destroying_driver_{};
static SwapchainEventSubject events_{};
static ThreadLocal<Swapchain> swapchain_;

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
      Swapchain::Publish<SwapchainCreatedEvent>();
    Swapchain::Publish<SwapchainInitEvent>(IsReinit());
  }
}

auto IsSwapchainInitialized() -> bool {
  return swapchain_.Get() != nullptr;
}

auto GetSwapchain() -> Swapchain* {
  ASSERT(IsSwapchainInitialized());
  return swapchain_;
}

auto OnSwapchainEvent() -> SwapchainEventObservable {
  return events_.get_observable();
}

void Swapchain::PublishEvent(SwapchainEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

auto Swapchain::CreateRenderPass() -> vk::RenderPass* {
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
  return builder.Build();
}

auto Swapchain::CreatePipeline() -> vk::RenderPipeline* {
  return vk::RenderPipeline::FromJson("swapchain.json");
}
}  // namespace prt