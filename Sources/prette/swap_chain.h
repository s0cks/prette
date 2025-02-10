#ifndef PRT_SWAP_CHAIN_H
#define PRT_SWAP_CHAIN_H

#include <vulkan/vulkan_core.h>

#include <limits>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  std::vector<VkSurfaceFormatKHR> surface_formats{};
  std::vector<VkPresentModeKHR> present_modes{};

  inline auto HasSurfaceFormats() const -> bool {
    return !surface_formats.empty();
  }

  inline auto HasPresentModes() const -> bool {
    return !present_modes.empty();
  }

  auto FindSurfaceFormat(const std::function<bool(const VkSurfaceFormatKHR&)>& filter) const -> const VkSurfaceFormatKHR& {
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

auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapChainSupportDetails;

static inline auto HasSwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> bool {
  return QuerySwapChainSupport(device, surface);
}

class SwapChain {
  friend class Runtime;
  friend class Renderer;
  DEFINE_NON_COPYABLE_TYPE(SwapChain);

 private:
  static void InitImageViews(const VkDevice& device);
  static void InitFramebuffers(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
  static void InitRenderPass(const VkDevice& device);

  static void InitSwapChain(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                            const VkAllocationCallbacks* allocator);
  static void ReInit(Driver* driver);
  static void DestroyFramebuffers(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyImageViews(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);
  static void DestroyRenderPass(Driver* driver);
  static void DestroySwapChain(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr);

 public:
  static auto GetVkSwapChain() -> const VkSwapchainKHR&;
  static auto GetVkImages() -> const std::vector<VkImage>&;
  static auto GetVkImageViews() -> const std::vector<VkImageView>&;
  static auto GetFormat() -> const VkFormat&;
  static auto GetExtent() -> const VkExtent2D&;
  static auto GetRenderPass() -> const VkRenderPass&;
  static auto GetFramebuffer(const uint32_t index) -> const VkFramebuffer&;

  static void Init(Driver* driver);
  static void Shutdown(Driver* driver);
};
}  // namespace prt

#endif  // PRT_SWAP_CHAIN_H
