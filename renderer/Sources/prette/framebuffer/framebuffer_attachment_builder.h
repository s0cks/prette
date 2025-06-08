#ifndef PRT_FRAMEBUFFER_ATTACHMENT_BUILDER_H
#define PRT_FRAMEBUFFER_ATTACHMENT_BUILDER_H

#include <vulkan/vulkan_core.h>

#include "prette/framebuffer/framebuffer_attachment.h"
namespace prt {
class FramebufferAttachmentBuilder {
 private:
  VkFormat format_;
  VkExtent2D extent_{};
  VkImageUsageFlags usage_{};
  VkImageAspectFlags aspect_{};
  VkImageLayout layout_{};

 public:
  explicit FramebufferAttachmentBuilder(const VkFormat format) :
    format_(format) {}
  ~FramebufferAttachmentBuilder() = default;

  auto GetFormat() const -> const VkFormat& {
    return format_;
  }

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  auto WithExtent(const VkExtent2D rhs) -> FramebufferAttachmentBuilder& {
    extent_ = rhs;
    return *this;
  }

  auto GetUsage() const -> const VkImageUsageFlags& {
    return usage_;
  }

  auto GetAspectMask() const -> const VkImageAspectFlags& {
    return aspect_;
  }

  auto IsValid() const -> bool;
  auto WithUsage(const VkImageUsageFlagBits rhs) -> FramebufferAttachmentBuilder&;
  auto Build() -> FramebufferAttachment*;

  auto operator()() -> FramebufferAttachment* {
    return Build();
  }

  operator FramebufferAttachment*() {
    return Build();
  }
};
}  // namespace prt

#endif  // PRT_FRAMEBUFFER_ATTACHMENT_BUILDER_H
