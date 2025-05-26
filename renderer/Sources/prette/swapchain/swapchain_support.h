#ifndef PRT_SWAPCHAIN_SUPPORT_H
#define PRT_SWAPCHAIN_SUPPORT_H

#include <algorithm>
#include <functional>
#include <vector>

#include "prette/gfx.h"
#include "prette/platform.h"
#include "prette/vk.h"

namespace prt {
struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  std::vector<VkSurfaceFormatKHR> surface_formats{};
  std::vector<VkPresentModeKHR> present_modes{};

  inline auto HasSurfaceFormats() const -> bool {
    return !surface_formats.empty();
  }

  inline auto HasPresentModes() const -> bool {
    return !present_modes.empty();
  }

  auto FindSurfaceFormat(const std::function<bool(const VkSurfaceFormatKHR&)>& filter) const
      -> const VkSurfaceFormatKHR& {
    const auto pos = std::ranges::find_if(surface_formats, filter);
    if (pos != std::end(surface_formats))
      return (*pos);
    DLOG(WARNING) << "failed to find valid surface format for swap chain.";
    return surface_formats[0];
  }

  auto FindPresentMode(const std::function<bool(const VkPresentModeKHR&)>& filter) const -> VkPresentModeKHR {
    const auto pos = std::ranges::find_if(present_modes, filter);
    if (pos != std::end(present_modes))
      return (*pos);
    DLOG(WARNING) << "failed to find valid present mode for swap chain.";
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  auto GetMaxImageCount() const -> uint32_t {
    return surface_capabilities.maxImageCount;
  }

  inline auto HasMaxImage() const -> bool {
    return GetMaxImageCount() > 0;
  }

  void ClampImageCount(uint32_t& image_count) const {
    if (HasMaxImage() && image_count > GetMaxImageCount())
      image_count = GetMaxImageCount();
  }

  auto GetExtent() const -> VkExtent2D;

  operator bool() const {
    return HasSurfaceFormats() && HasPresentModes();
  }
};

auto QuerySwapchainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapchainSupportDetails;

static inline auto HasSwapchainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> bool {
  return QuerySwapchainSupport(device, surface);
}
}  // namespace prt

#endif  // PRT_SWAPCHAIN_SUPPORT_H
