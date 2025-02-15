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
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
static SwapChainEventSubject events_{};
static RelaxedAtomic<bool> resized_(false);
static rx::subscription on_window_resized_{};
static std::array<SwapChainFrame, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> frames_{};
static RelaxedAtomic<uint32_t> frame_(0);
static uint32_t num_images_ = 0;

static GraphicsPipeline pipeline_{};
static VkRenderPass pass_{};
static VkSwapchainKHR swapchain_{};
static VkFormat format_{};
static VkExtent2D extent_{};

static std::vector<VkImage> images_{};
static std::vector<VkImageView> views_{};
static std::vector<VkFramebuffer> framebuffers_{};

static VkDescriptorPool descriptor_pool_{};
static VkDescriptorSetLayout descriptor_set_layout_{};
static std::vector<VkDescriptorSet> descriptor_sets_{};

SwapChainFrame::SwapChainFrame(const uint32_t f, const Driver* driver) :
  frame(f) {
  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  CHECK_VK(FATAL, vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &available),
           "failed to create available semaphore");
  CHECK_VK(FATAL, vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &finished),
           "failed to create finished semaphore");
  CHECK_VK(FATAL, vkCreateFence(driver->GetDevice(), &fence_info, driver->GetAllocator(), &fence), "failed to create fence");
}

void SwapChainFrame::Destroy(const Driver* driver) {
  ASSERT(driver);
  vkDestroySemaphore(driver->GetDevice(), finished, driver->GetAllocator());
  vkDestroySemaphore(driver->GetDevice(), available, driver->GetAllocator());
  vkDestroyFence(driver->GetDevice(), fence, driver->GetAllocator());
}

auto SwapChainInitEvent::ToString() const -> std::string {
  ToStringHelper<SwapChainInitEvent> helper{};
  return helper;
}

auto SwapChainDestroyedEvent::ToString() const -> std::string {
  ToStringHelper<SwapChainDestroyedEvent> helper{};
  return helper;
}

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

auto SwapChainSupportDetails::GetExtent() const -> VkExtent2D {
  if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  }

  const auto window = GetAppWindow();
  ASSERT(window);
  VkExtent2D actual = window->GetFramebufferSize();
  return ClampExtent(actual, surface_capabilities);
}

auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapChainSupportDetails {
  SwapChainSupportDetails details{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.surface_capabilities);

  uint32_t num_formats = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &num_formats, nullptr);
  if (num_formats > 0) {
    details.surface_formats.resize(num_formats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &num_formats, &details.surface_formats[0]);
  }

  uint32_t num_present_modes = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &num_present_modes, nullptr);
  if (num_present_modes > 0) {
    details.present_modes.resize(num_present_modes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &num_present_modes, &details.present_modes[0]);
  }
  return details;
}

static inline auto IsValidSwapChainSurfaceFormat(const VkSurfaceFormatKHR& format) -> bool {
  return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
}

static inline auto IsValidSwapChainPresentMode(const VkPresentModeKHR& mode) -> bool {
  return mode == VK_PRESENT_MODE_MAILBOX_KHR;
}

void SwapChain::InitPipeline(const Driver* driver) {
  ASSERT(driver);
  std::vector<VkDynamicState> dynamic_states = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
  };
  pipeline_ = GraphicsPipeline(driver, "swapchain", pass_, dynamic_states, SwapChain::GetExtent(), {});
}

void SwapChain::InitDescriptorSets(const Driver* driver, const uint64_t num_sets) {
  ASSERT(driver);
  descriptor_sets_.resize(num_sets);
  std::vector<VkDescriptorSetLayout> layouts(num_sets, descriptor_set_layout_);
  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor_pool_;
  alloc_info.descriptorSetCount = num_sets;
  alloc_info.pSetLayouts = layouts.data();
  CHECK_VK(FATAL, vkAllocateDescriptorSets(driver->GetDevice(), &alloc_info, descriptor_sets_.data()),
           "failed to allocate descriptor sets");
}

void SwapChain::InitRenderPass(const Driver* driver) {
  ASSERT(driver);
  std::array<VkAttachmentDescription, 1> attachments = {};
  // Color attachment
  attachments[0].format = SwapChain::GetFormat();
  attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
  attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorReference = {};
  colorReference.attachment = 0;
  colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDescription = {};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.colorAttachmentCount = 1;
  subpassDescription.pColorAttachments = &colorReference;
  subpassDescription.pDepthStencilAttachment = nullptr;
  subpassDescription.inputAttachmentCount = 0;
  subpassDescription.pInputAttachments = nullptr;
  subpassDescription.preserveAttachmentCount = 0;
  subpassDescription.pPreserveAttachments = nullptr;
  subpassDescription.pResolveAttachments = nullptr;

  // Subpass dependencies for layout transitions
  std::array<VkSubpassDependency, 1> dependencies{};
  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0;
  dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  VkRenderPassCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = attachments.size();
  create_info.pAttachments = attachments.data();
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpassDescription;
  create_info.dependencyCount = dependencies.size();
  create_info.pDependencies = dependencies.data();
  CHECK_VK(FATAL, vkCreateRenderPass(driver->GetDevice(), &create_info, driver->GetAllocator(), &pass_),
           "failed to create vk render pass");
}

auto SwapChain::GetRenderPass() -> VkRenderPass const& {
  return pass_;
}

auto SwapChain::GetGraphicsPipeline() -> GraphicsPipeline const& {
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
  if (!is_reinit) {
    for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
      frames_.at(idx) = SwapChainFrame(idx, driver);
    }
  }
  Publish<SwapChainInitEvent>(is_reinit);
}

void SwapChain::DestroySwap(const Driver* driver, const bool is_reinit) {
  ASSERT(driver);
  for (const auto& view : views_) {
    vkDestroyImageView(driver->GetDevice(), view, driver->GetAllocator());
  }
  for (const auto& framebuffer : framebuffers_) {
    vkDestroyFramebuffer(driver->GetDevice(), framebuffer, driver->GetAllocator());
  }
  pipeline_.Destroy(driver);
  vkDestroySwapchainKHR(driver->GetDevice(), swapchain_, driver->GetAllocator());
  if (!is_reinit) {
    vkDestroyRenderPass(driver->GetDevice(), pass_, driver->GetAllocator());
    for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
      frames_.at(idx).Destroy(driver);
    }
  }
  Publish<SwapChainDestroyedEvent>(is_reinit);
}

auto SwapChain::GetFramebuffer(const uint32_t idx) -> VkFramebuffer const& {
  return framebuffers_[idx];
}

void SwapChain::ReInitSwap(const Driver* driver) {
  ASSERT(driver);
  const auto window = GetAppWindow();
  ASSERT(window);
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window->GetHandle(), &width, &height);
  while (width == 0 && height == 0) {
    glfwGetFramebufferSize(window->GetHandle(), &width, &height);
    glfwWaitEvents();
  }

  driver->WaitDeviceIdle();
  DestroySwap(driver, true);
  InitSwap(driver, true);
}

auto SwapChain::AcquireNextImage(const Driver* driver) -> bool {
  ASSERT(driver);
  auto& frame = frames_.at((uint32_t)frame_);
  vkWaitForFences(driver->GetDevice(), 1, &frame.fence, VK_TRUE, UINT64_MAX);
  const auto status =
      vkAcquireNextImageKHR(driver->GetDevice(), GetSwapChain(), UINT64_MAX, frame.available, VK_NULL_HANDLE, &frame.image);
  if (status == VK_ERROR_OUT_OF_DATE_KHR) {
    ReInitSwap(driver);
    return false;
  } else if (status != VK_SUCCESS && status != VK_SUBOPTIMAL_KHR) {
    LOG(FATAL) << "failed to acquire swap chain image:" << string_VkResult(status);
  }
  vkResetFences(driver->GetDevice(), 1, &frame.fence);
  return true;
}

void SwapChain::InitImages(const Driver* driver) {
  vkGetSwapchainImagesKHR(driver->GetDevice(), swapchain_, &num_images_, nullptr);
  images_.resize(num_images_);
  vkGetSwapchainImagesKHR(driver->GetDevice(), swapchain_, &num_images_, &images_[0]);
  {
    views_.resize(num_images_);
    for (auto idx = 0; idx < num_images_; idx++) {
      VkImageViewCreateInfo create_info{};
      create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      create_info.image = images_[idx];
      create_info.format = format_;
      create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      create_info.subresourceRange.baseMipLevel = 0;
      create_info.subresourceRange.levelCount = 1;
      create_info.subresourceRange.baseArrayLayer = 0;
      create_info.subresourceRange.layerCount = 1;
      CHECK_VK(FATAL, vkCreateImageView(driver->GetDevice(), &create_info, driver->GetAllocator(), &views_[idx]),
               "failed to create vk image view");
    }
  }
  {
    framebuffers_.resize(num_images_);
    for (auto idx = 0; idx < num_images_; idx++) {
      std::array<VkImageView, 1> attachments = {
          views_[idx],
      };
      VkFramebufferCreateInfo create_info{};
      create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      create_info.renderPass = pass_;
      create_info.attachmentCount = attachments.size();
      create_info.pAttachments = attachments.data();
      create_info.width = extent_.width;
      create_info.height = extent_.height;
      create_info.layers = 1;
      CHECK_VK(FATAL, vkCreateFramebuffer(driver->GetDevice(), &create_info, driver->GetAllocator(), &framebuffers_[idx]),
               "failed to create vk framebuffer");
    }
  }
}

void SwapChain::Submit(const Driver* driver, const std::vector<VkCommandBuffer>& cmd_buffers) {
  ASSERT(driver);
  auto& frame = GetCurrentFrame();
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  std::array<VkSemaphore, 1> wait_semaphores = {frame.available};
  std::array<VkPipelineStageFlags, 1> wait_stages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = wait_semaphores.size();
  submit_info.pWaitSemaphores = wait_semaphores.data();
  submit_info.pWaitDstStageMask = wait_stages.data();
  submit_info.commandBufferCount = cmd_buffers.size();
  submit_info.pCommandBuffers = cmd_buffers.data();
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &frame.finished;
  CHECK_VK(FATAL, vkQueueSubmit(driver->GetGraphicsQueue(), 1, &submit_info, frame.fence),
           "failed to submit to vk graphics queue");
}

void SwapChain::Present(const Driver* driver) {
  ASSERT(driver);
  auto& frame = GetCurrentFrame();
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &frame.finished;
  present_info.pSwapchains = &GetSwapChain();
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &frame.image;
  const auto result = vkQueuePresentKHR(driver->GetPresentQueue(), &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized_) {
    resized_ = false;
    ReInitSwap(driver);
  }
  frame_ = ((((uint32_t)frame_) + 1) % MAX_NUMBER_OF_FRAMES_IN_FLIGHT);
}

auto SwapChain::GetImageViews() -> const std::vector<VkImageView>& {
  return views_;
}

auto SwapChain::GetCurrentFrame() -> SwapChainFrame const& {
  return frames_.at((uint32_t)frame_);
}

auto SwapChain::GetImageView(const uint32_t idx) -> VkImageView const& {
  return views_[idx];
}

static auto GetFramebuffer(const uint32_t idx) -> VkFramebuffer const& {
  return framebuffers_[idx];
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
  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    frames_.at(idx).frame = idx;
  }
  OnDriverInitEvent().subscribe([](DriverInitEvent* event) {
    ASSERT(event);
    const auto driver = event->GetDriver();
    ASSERT(driver);
    InitWindowResizeListener();
    InitSwap(driver, false);
  });
  OnDestroyingDriverEvent().subscribe([](DestroyingDriverEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    DestroySwap(driver, false);
  });
}
}  // namespace prt