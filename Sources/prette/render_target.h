#ifndef PRT_RENDER_TARGET_H
#define PRT_RENDER_TARGET_H

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
class RenderTarget {
  DEFINE_DEFAULT_COPYABLE_TYPE(RenderTarget);

 private:
  VkImage image_{};
  VkImageView view_{};
  VkDeviceMemory memory_{};
  VkFramebuffer framebuffer_{};
  VkExtent2D extent_{};

 public:
  RenderTarget() = default;
  RenderTarget(const VkRenderPass& pass, const VkExtent2D& extent, const VkFormat format = VK_FORMAT_B8G8R8A8_SRGB);
  ~RenderTarget() = default;

  auto GetImage() const -> VkImage const& {
    return image_;
  }

  auto GetView() const -> VkImageView const& {
    return view_;
  }

  auto GetMemory() const -> VkDeviceMemory const& {
    return memory_;
  }

  auto GetFramebuffer() const -> VkFramebuffer const& {
    return framebuffer_;
  }

  auto GetExtent() const -> VkExtent2D const& {
    return extent_;
  }

  void Destroy() const;
};
}  // namespace prt

#endif  // PRT_RENDER_TARGET_H
