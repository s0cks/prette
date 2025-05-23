#ifndef PRT_SWAPCHAIN_BUILDER_H
#define PRT_SWAPCHAIN_BUILDER_H

#include "prette/common.h"
#include "prette/gfx_vk.h"
#include "prette/vk.h"

namespace prt {
class Swapchain;
class SwapchainBuilder : public vk::HandleBuilderTemplate<VkSwapchainCreateInfoKHR, Swapchain> {
  using ParentType = vk::HandleBuilderTemplate<VkSwapchainCreateInfoKHR, Swapchain>;

 private:
  QueueFamilyIndices queue_families_{};

 public:
  SwapchainBuilder();
  ~SwapchainBuilder() override = default;

  auto WithSurface(const VkSurfaceKHR rhs) -> SwapchainBuilder& {
    ASSERT(rhs != VK_NULL_HANDLE);
    info_ptr()->surface = rhs;
    return *this;
  }

  auto WithSurface(vk::Surface* rhs) -> SwapchainBuilder&;

  auto WithMinImageCount(const uint32_t rhs) -> SwapchainBuilder& {
    ASSERT(rhs >= 1);
    info_ptr()->minImageCount = rhs;
    return *this;
  }

  auto WithFormat(const VkFormat rhs) -> SwapchainBuilder& {
    info_ptr()->imageFormat = rhs;
    return *this;
  }

  auto WithColorSpace(const VkColorSpaceKHR rhs) -> SwapchainBuilder& {
    info_ptr()->imageColorSpace = rhs;
    return *this;
  }

  auto WithSurfaceFormat(const VkSurfaceFormatKHR rhs) -> SwapchainBuilder& {
    info_ptr()->imageFormat = rhs.format;
    info_ptr()->imageColorSpace = rhs.colorSpace;
    return *this;
  }

  auto WithImageExtent(const VkExtent2D rhs) -> SwapchainBuilder& {
    info_ptr()->imageExtent = rhs;
    return *this;
  }

  auto WithImageArrayLayers(const uint32_t rhs) -> SwapchainBuilder& {
    ASSERT(rhs >= 1);
    info_ptr()->imageArrayLayers = rhs;
    return *this;
  }

  auto WithImageUsage(const VkImageUsageFlags rhs) -> SwapchainBuilder& {
    info_ptr()->imageUsage = rhs;
    return *this;
  }

  inline auto WithColorImageUsage() -> SwapchainBuilder& {
    info_ptr()->imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    return *this;
  }

  auto WithCompositeAlpha(const VkCompositeAlphaFlagBitsKHR rhs) -> SwapchainBuilder& {
    info_ptr()->compositeAlpha = rhs;
    return *this;
  }

  auto WithClipped(const bool rhs) -> SwapchainBuilder& {
    info_ptr()->clipped = rhs;
    return *this;
  }

  inline auto WithClippedEnabled() -> SwapchainBuilder& {
    return WithClipped(true);
  }

  inline auto WithClippedDisabled() -> SwapchainBuilder& {
    return WithClipped(false);
  }

  auto WithOldSwapchain(const VkSwapchainKHR rhs) -> SwapchainBuilder& {
    info_ptr()->oldSwapchain = rhs;
    return *this;
  }

  auto WithQueueFamilies(const QueueFamilyIndices& rhs) -> SwapchainBuilder&;

  auto IsValid() const -> bool override;
  auto Build() -> Swapchain* override;
};
}  // namespace prt

#endif  // PRT_SWAPCHAIN_BUILDER_H
