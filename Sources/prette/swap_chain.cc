#include "prette/swap_chain.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"
#include "prette/window.h"

namespace prt {
static VkSwapchainKHR chain_{};
static std::vector<VkImage> images_{};
static std::vector<VkImageView> views_{};
static std::vector<VkFramebuffer> framebuffers_{};
static VkRenderPass render_pass_;
static VkFormat format_{};
static VkExtent2D extent_{};

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

auto SwapChainSupportDetails::GetExtent() const -> VkExtent2D {
  if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  }

  const auto window = GetAppWindow();
  ASSERT(window);
  VkExtent2D actual = window->GetFramebufferSize();
  return ClampExtent(actual, surface_capabilities);
}

static inline auto IsValidSwapChainSurfaceFormat(const VkSurfaceFormatKHR& format) -> bool {
  return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
}

static inline auto IsValidSwapChainPresentMode(const VkPresentModeKHR& mode) -> bool {
  return mode == VK_PRESENT_MODE_MAILBOX_KHR;
}

auto SwapChain::GetRenderPass() -> const VkRenderPass& {
  return render_pass_;
}

auto SwapChain::GetFramebuffer(const uint32_t frame) -> const VkFramebuffer& {
  return framebuffers_.at(frame);
}

auto SwapChain::GetVkSwapChain() -> const VkSwapchainKHR& {
  return chain_;
}

auto SwapChain::GetExtent() -> const VkExtent2D& {
  return extent_;
}

void SwapChain::InitRenderPass(const VkDevice& device) {
  VkAttachmentDescription color_attachment{};
  color_attachment.format = format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass;
  create_info.dependencyCount = 1;
  create_info.pDependencies = &dependency;

  CHECK_VK(FATAL, vkCreateRenderPass(device, &create_info, nullptr, &render_pass_), "failed to create vk render pass");
}

void SwapChain::InitImageViews(const VkDevice& device) {
  views_.resize(images_.size());
  for (auto idx = 0; idx < images_.size(); idx++) {
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
    CHECK_VK(FATAL, vkCreateImageView(device, &create_info, nullptr, &views_[idx]), "failed to create vk image view");
  }
}

void SwapChain::InitFramebuffers(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  framebuffers_.resize(views_.size());
  for (auto idx = 0; idx < views_.size(); idx++) {
    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = GetRenderPass();
    create_info.attachmentCount = 1;
    create_info.pAttachments = &views_[idx];
    create_info.width = GetExtent().width;
    create_info.height = GetExtent().height;
    create_info.layers = 1;
    CHECK_VK(FATAL, vkCreateFramebuffer(device, &create_info, allocator, &framebuffers_[idx]),
             "failed to create vk framebuffers");
  }
}

void SwapChain::Recreate(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface) {
  const auto window = GetAppWindow();
  ASSERT(window);
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window->GetHandle(), &width, &height);
  while (width == 0 && height == 0) {
    glfwGetFramebufferSize(window->GetHandle(), &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device);
  DestroySwapChain(device);
  SwapChain::Init(physical_device, device, surface);
}

void SwapChain::Init(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                     const VkAllocationCallbacks* allocator) {
  const auto support = QuerySwapChainSupport(physical_device, surface);
  const auto surface_format = support.FindSurfaceFormat(&IsValidSwapChainSurfaceFormat);
  const auto present_mode = support.FindPresentMode(&IsValidSwapChainPresentMode);
  const auto extent = support.GetExtent();

  uint32_t image_count = support.surface_capabilities.minImageCount + 1;
  support.ClampImageCount(image_count);

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const auto indices = FindQueueFamilies(physical_device, surface);
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
  CHECK_VK(FATAL, vkCreateSwapchainKHR(device, &create_info, nullptr, &chain_), "failed to create vk swap chain");

  vkGetSwapchainImagesKHR(device, chain_, &image_count, nullptr);
  images_.resize(image_count);
  vkGetSwapchainImagesKHR(device, chain_, &image_count, &images_[0]);

  format_ = surface_format.format;
  extent_ = extent;
  InitImageViews(device);
  InitRenderPass(device);
  InitFramebuffers(device);
}

void SwapChain::DestroySwapChain(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  DestroyFramebuffers(device, allocator);
  DestroyImageViews(device, allocator);
  vkDestroySwapchainKHR(device, chain_, allocator);
}

void SwapChain::DestroyFramebuffers(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  for (const auto& framebuffer : framebuffers_) {
    vkDestroyFramebuffer(device, framebuffer, allocator);
  }
}

void SwapChain::DestroyImageViews(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  for (const auto& view : views_) {
    vkDestroyImageView(device, view, allocator);
  }
}

void SwapChain::DestroyRenderPass(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  vkDestroyRenderPass(device, render_pass_, allocator);
}

void SwapChain::Shutdown(const VkDevice& device, const VkAllocationCallbacks* allocator) {
  DestroySwapChain(device, allocator);
  DestroyRenderPass(device, allocator);
}
}  // namespace prt