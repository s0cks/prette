#include "prette/framebuffer/framebuffer_attachment.h"

#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/image/image.h"       // IWYU pragma: keep
#include "prette/image/image_view.h"  // IWYU pragma: keep

namespace prt {
FramebufferAttachment::FramebufferAttachment(VkFormat format, vk::Image* image, vk::ImageView* view) :
  format_(format),
  image_(image),
  view_(view) {
  ASSERT_INITIALIZED(image);
  ASSERT_INITIALIZED(view);
}

FramebufferAttachment::~FramebufferAttachment() {
  delete image_;
  delete view_;
}
}  // namespace prt