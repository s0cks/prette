#ifndef PRT_RENDER_TARGET_H
#define PRT_RENDER_TARGET_H

#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/framebuffer/framebuffer.h"
#include "prette/image/image.h"
#include "prette/vk.h"

namespace prt {
static constexpr const auto kDefaultRenderTargetFormat = VK_FORMAT_B8G8R8A8_SRGB;
class RenderTarget {
  DEFINE_NON_COPYABLE_TYPE(RenderTarget);

 private:
  uint64_t index_ = 0;
  VkExtent2D extent_{};
  vk::Image* image_ = nullptr;
  vk::ImageView* view_ = nullptr;
  Framebuffer* framebuffer_ = nullptr;

 public:
  RenderTarget(const uint64_t index, vk::RenderPass* pass, const VkExtent2D& extent,
               const VkFormat format = kDefaultRenderTargetFormat, vk::ImageView* depth_view = nullptr);
  ~RenderTarget();

  auto GetImage() const -> vk::Image* {
    return image_;
  }

  auto GetView() const -> vk::ImageView* {
    return view_;
  }

  auto GetFramebuffer() const -> Framebuffer* {
    return framebuffer_;
  }

  auto GetExtent() const -> VkExtent2D const& {
    return extent_;
  }

  auto IsInitialized() const -> bool;
};

class RenderTargetSet {
 private:
  std::vector<RenderTarget*> targets_{};

 public:
  RenderTargetSet(const uint64_t num_targets, vk::RenderPass* pass, const VkExtent2D& extent, const VkFormat format,
                  vk::ImageView* depth_view = nullptr);
  ~RenderTargetSet();
};
}  // namespace prt

#endif  // PRT_RENDER_TARGET_H
