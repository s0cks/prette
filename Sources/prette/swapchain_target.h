#ifndef PRT_SWAPCHAIN_TARGET_H
#define PRT_SWAPCHAIN_TARGET_H

#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
class SwapChainTarget {
  DEFINE_DEFAULT_COPYABLE_TYPE(SwapChainTarget);

 private:
  uint64_t index_;
  VkImageView view_ = VK_NULL_HANDLE;
  VkFramebuffer framebuffer_ = VK_NULL_HANDLE;

 public:
  SwapChainTarget(const uint64_t idx, const VkFormat& format, const VkRenderPass& pass, const VkExtent2D& extent,
                  const VkImage& image);
  ~SwapChainTarget();

  auto GetIndex() const -> uint64_t {
    return index_;
  }

  auto GetView() const -> const VkImageView& {
    return view_;
  }

  auto GetFramebuffer() const -> const VkFramebuffer& {
    return framebuffer_;
  }

  auto ToString() const -> std::string;

  friend auto operator<<(std::ostream& stream, const SwapChainTarget& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};
}  // namespace prt

#endif  // PRT_SWAPCHAIN_TARGET_H
