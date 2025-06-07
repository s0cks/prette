#ifndef PRT_GBUFFER_H
#define PRT_GBUFFER_H

#include <array>
#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/framebuffer/framebuffer.h"
#include "prette/framebuffer/framebuffer_attachment.h"

namespace prt {
class GBuffer {
 public:
  enum ColorAttachment : uint32_t {
    kPosition = 0,
    kNormals,
    kAlbedo,
    kTotalNumberOfColorAttachments,
  };

  using ColorAttachmentArray = std::array<ColorAttachment*, kTotalNumberOfColorAttachments>;

 private:
  VkExtent2D extent_{};
  Framebuffer* framebuffer_ = nullptr;
  ColorAttachmentArray color_attachments_{};
  FramebufferAttachment* depth_attachment_ = nullptr;

 public:
  explicit GBuffer(const VkExtent2D extent);
  ~GBuffer();

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  auto GetFramebuffer() const -> Framebuffer* {
    return framebuffer_;
  }

  auto GetColorAttachments() const -> const ColorAttachmentArray& {
    return color_attachments_;
  }

  auto GetDepthAttachment() const -> FramebufferAttachment* {
    return depth_attachment_;
  }
};
}  // namespace prt

#endif  // PRT_GBUFFER_H
