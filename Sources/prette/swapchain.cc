#include "prette/swapchain.h"

#include <sys/param.h>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_vk.h"
#include "prette/pipeline.h"
#include "prette/relaxed_atomic.h"
#include "prette/renderer.h"
#include "prette/swapchain_render_pass.h"
#include "prette/swapchain_support.h"
#include "prette/swapchain_target.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
static SwapChainEventSubject events_{};
static RelaxedAtomic<bool> resized_(false);
static rx::subscription on_window_resized_{};
static std::array<SwapChainFrame*, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> frames_{};
static std::vector<SwapChainTarget*> targets_{};
static RelaxedAtomic<uint32_t> frame_(0);

static SwapChainRenderPass* render_pass_ = nullptr;
static GraphicsPipeline* pipeline_ = nullptr;
static VkSwapchainKHR swapchain_{};
static VkFormat format_{};
static VkExtent2D extent_{};

static std::vector<VkImage> images_{};

auto OnSwapChainEvent() -> SwapChainEventObservable {
  return events_.get_observable();
}

static inline void PublishEvent(SwapChainEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

void SwapChain::InitWindowResizeListener() {
  if (on_window_resized_.is_subscribed())
    on_window_resized_.unsubscribe();
  const auto window = GetAppWindow();
  ASSERT(window);
  on_window_resized_ = window->OnWindowSizeEvent().subscribe([](WindowSizeEvent* event) {
    ASSERT(event);
    resized_ = true;
  });
}

static inline auto IsValidSwapChainSurfaceFormat(const VkSurfaceFormatKHR& format) -> bool {
  return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
}

static inline auto IsValidSwapChainPresentMode(const VkPresentModeKHR& mode) -> bool {
  return mode == VK_PRESENT_MODE_MAILBOX_KHR;
}

void SwapChain::InitPipeline(const Driver* driver) {
  ASSERT(driver);
  pipeline_ = GraphicsPipeline::FromJson("/Users/tazz/Projects/prette/prette/resources/pipelines/swapchain.json",
                                         render_pass_->Get(), SwapChain::GetExtent());
}

void SwapChain::InitRenderPass(const Driver* driver) {
  ASSERT(driver);
  render_pass_ = new SwapChainRenderPass(format_);
}

auto SwapChain::GetRenderPass() -> SwapChainRenderPass* {
  return render_pass_;
}

auto SwapChain::GetGraphicsPipeline() -> GraphicsPipeline* const& {
  return pipeline_;
}

void SwapChain::InitSwap(const Driver* driver, const bool is_reinit) {
  ASSERT(driver);
  const auto support = QuerySwapChainSupport(driver->GetPhysicalDevice(), driver->GetSurface());
  const auto surface_format = support.FindSurfaceFormat(&IsValidSwapChainSurfaceFormat);
  const auto present_mode = support.FindPresentMode(&IsValidSwapChainPresentMode);
  const auto extent = support.GetExtent();

  uint32_t image_count = support.surface_capabilities.minImageCount + 1;
  support.ClampImageCount(image_count);

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = driver->GetSurface();
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (support.surface_capabilities.supportedTransforms & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }
  if (support.surface_capabilities.supportedTransforms & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  const auto indices = FindQueueFamilies(driver->GetPhysicalDevice(), driver->GetSurface());
  const std::array<uint32_t, 2> families = {
      indices.GetGraphicsFamily(),
      indices.GetPresentFamily(),
  };
  if (indices.GetGraphicsFamily() != indices.GetPresentFamily()) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = &families[0];
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  create_info.preTransform = support.surface_capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;
  CHECK_VK(FATAL, vkCreateSwapchainKHR(driver->GetDevice(), &create_info, nullptr, &swapchain_),
           "failed to create vk swap chain");
  format_ = surface_format.format;
  extent_ = extent;
  InitRenderPass(driver);
  InitPipeline(driver);
  InitImages(driver);
  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    if (!is_reinit) {
      frames_.at(idx) = new SwapChainFrame(idx);
    } else {
      frames_.at(idx)->SetFrame(idx);
    }
  }
  Publish<SwapChainInitEvent>(is_reinit);
}

void SwapChain::DestroySwap(const Driver* driver, const bool is_reinit) {
  ASSERT(driver);
  delete pipeline_;
  vkDestroySwapchainKHR(driver->GetDevice(), swapchain_, driver->GetAllocator());
  if (!is_reinit) {
    delete render_pass_;
    std::ranges::for_each(frames_, [](SwapChainFrame* frame) {
      delete frame;
    });
    std::ranges::for_each(targets_, [](SwapChainTarget* target) {
      delete target;
    });
  }
  Publish<SwapChainDestroyedEvent>(is_reinit);
}

void SwapChain::ReInitSwap(const Driver* driver) {
  ASSERT(driver);
  const auto window = GetAppWindow();
  ASSERT(window);
  driver->WaitDeviceIdle();
  DestroySwap(driver, true);
  InitSwap(driver, true);
}

auto SwapChain::AcquireNextImage(const Driver* driver) -> bool {
  ASSERT(driver);
  auto& frame = frames_.at((uint32_t)frame_);
  vkWaitForFences(driver->GetDevice(), 1, &frame->GetFence(), VK_TRUE, UINT64_MAX);
  const auto status = vkAcquireNextImageKHR(driver->GetDevice(), GetSwapChain(), UINT64_MAX, frame->GetAvailableSemaphore(),
                                            VK_NULL_HANDLE, &frame->image_);
  if (status == VK_ERROR_OUT_OF_DATE_KHR) {
    ReInitSwap(driver);
    return false;
  } else if (status != VK_SUCCESS && status != VK_SUBOPTIMAL_KHR) {
    LOG(FATAL) << "failed to acquire swap chain image:" << string_VkResult(status);
  }
  vkResetFences(driver->GetDevice(), 1, &frame->GetFence());
  return true;
}

void SwapChain::InitImages(const Driver* driver) {
  uint32_t num_images = 0;
  vkGetSwapchainImagesKHR(driver->GetDevice(), swapchain_, &num_images, nullptr);
  images_.resize(num_images);
  vkGetSwapchainImagesKHR(driver->GetDevice(), swapchain_, &num_images, &images_[0]);
  targets_.resize(num_images);
  for (auto idx = 0; idx < num_images; idx++) {
    targets_[idx] = new SwapChainTarget(idx, format_, render_pass_->Get(), extent_, images_[idx]);
  }
}

void SwapChain::Submit(const Driver* driver, const std::vector<VkCommandBuffer>& cmd_buffers) {
  ASSERT(driver);
  const auto frame = GetCurrentFrame();
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  std::array<VkSemaphore, 1> wait_semaphores = {frame->GetAvailableSemaphore()};
  std::array<VkPipelineStageFlags, 1> wait_stages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = wait_semaphores.size();
  submit_info.pWaitSemaphores = wait_semaphores.data();
  submit_info.pWaitDstStageMask = wait_stages.data();
  submit_info.commandBufferCount = cmd_buffers.size();
  submit_info.pCommandBuffers = cmd_buffers.data();
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &frame->GetFinishedSemaphore();
  CHECK_VK(FATAL, vkQueueSubmit(driver->GetGraphicsQueue(), 1, &submit_info, frame->GetFence()),
           "failed to submit to vk graphics queue");
}

void SwapChain::Present(const Driver* driver) {
  ASSERT(driver);
  const auto frame = GetCurrentFrame();
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &frame->GetFinishedSemaphore();
  present_info.pSwapchains = &GetSwapChain();
  present_info.swapchainCount = 1;
  std::array<uint32_t, 1> images = {
      frame->GetImage(),
  };
  present_info.pImageIndices = images.data();
  const auto result = vkQueuePresentKHR(driver->GetPresentQueue(), &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized_) {
    resized_ = false;
    ReInitSwap(driver);
  }
  frame_ = ((((uint32_t)frame_) + 1) % MAX_NUMBER_OF_FRAMES_IN_FLIGHT);
}

auto SwapChain::GetCurrentFrame() -> SwapChainFrame* {
  return frames_.at((uint32_t)frame_);
}

auto SwapChain::GetFramebuffer(const uint32_t idx) -> VkFramebuffer const& {
  ASSERT(idx >= 0 && idx < targets_.size());
  return targets_[idx]->GetFramebuffer();
}

auto SwapChain::GetImageView(const uint32_t idx) -> VkImageView const& {
  ASSERT(idx >= 0 && idx < targets_.size());
  return targets_[idx]->GetView();
}

auto SwapChain::GetImages() -> const std::vector<VkImage>& {
  return images_;
}

auto SwapChain::GetFormat() -> VkFormat const& {
  return format_;
}

auto SwapChain::GetExtent() -> VkExtent2D const& {
  return extent_;
}

auto SwapChain::GetSwapChain() -> VkSwapchainKHR const& {
  return swapchain_;
}

void SwapChain::Init() {
  OnDriverInitEvent().subscribe([](DriverInitEvent* event) {
    ASSERT(event);
    InitWindowResizeListener();
    InitSwap(Driver::Get(), false);
  });
  OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    DestroySwap(driver, false);
  });
}
}  // namespace prt