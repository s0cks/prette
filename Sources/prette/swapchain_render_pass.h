#ifndef PRT_SWAPCHAIN_RENDER_PASS_H
#define PRT_SWAPCHAIN_RENDER_PASS_H

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"

namespace prt {
class SwapChainRenderPass {
 private:
  VkRenderPass pass_ = VK_NULL_HANDLE;

 public:
  explicit SwapChainRenderPass(const VkFormat& format);
  ~SwapChainRenderPass();

  auto Get() const -> const VkRenderPass& {
    return pass_;
  }
};
}  // namespace prt

#endif  // PRT_SWAPCHAIN_RENDER_PASS_H
