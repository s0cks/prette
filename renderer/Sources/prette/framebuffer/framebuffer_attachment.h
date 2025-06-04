#ifndef PRT_FRAMEBUFFER_ATTACHMENT_H
#define PRT_FRAMEBUFFER_ATTACHMENT_H

#include "prette/assertions.h"
#include "prette/vk.h"

namespace prt {
class FramebufferAttachment;
class FramebufferAttachmentVisitor {
 protected:
  FramebufferAttachmentVisitor() = default;

 public:
  virtual ~FramebufferAttachmentVisitor() = default;
  virtual auto Visit(FramebufferAttachment* rhs) -> bool = 0;
};

class FramebufferAttachment {
  friend class Framebuffer;
  friend class FramebufferBuilder;
  friend class FramebufferAttachmentBuilder;

 private:
  VkFormat format_;
  vk::Image* image_;
  vk::ImageView* view_;

  FramebufferAttachment(VkFormat format, vk::Image* image, vk::ImageView* view);

 public:
  ~FramebufferAttachment();

  auto GetFormat() const -> const VkFormat& {
    return format_;
  }

  auto GetImage() const -> vk::Image* {
    return image_;
  }

  auto GetImageView() const -> vk::ImageView* {
    return view_;
  }

  auto Accept(FramebufferAttachmentVisitor* vis) -> bool {
    ASSERT(vis);
    return vis->Visit(this);
  }
};
}  // namespace prt

#endif  // PRT_FRAMEBUFFER_ATTACHMENT_H
