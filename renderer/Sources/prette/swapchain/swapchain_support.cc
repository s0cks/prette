#include "prette/swapchain/swapchain_support.h"

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/platform.h"
#include "prette/vk.h"
#include "prette/window/window.h"

namespace prt {

auto SwapchainSupportDetails::GetExtent() const -> VkExtent2D {
  if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return surface_capabilities.currentExtent;
  }

  const auto window = GetAppWindow();
  ASSERT(window);
  VkExtent2D actual = window->GetFramebufferSize();
  return ClampExtent(actual, surface_capabilities);
}

auto QuerySwapchainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapchainSupportDetails {
  SwapchainSupportDetails details{};
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
}  // namespace prt