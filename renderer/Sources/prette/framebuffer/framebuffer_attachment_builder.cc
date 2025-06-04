#include "prette/framebuffer/framebuffer_attachment_builder.h"

#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/framebuffer/framebuffer_attachment.h"
#include "prette/image/image.h"
#include "prette/image/image_builder.h"
#include "prette/image/image_view.h"
#include "prette/image/image_view_builder.h"
#include "prette/vk.h"

namespace prt {
auto FramebufferAttachmentBuilder::WithUsage(const VkImageUsageFlags rhs) -> FramebufferAttachmentBuilder& {
  usage_ = rhs;
  if ((rhs & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) {
    aspect_ = VK_IMAGE_ASPECT_COLOR_BIT;
    layout_ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  } else if ((rhs & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
    aspect_ = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (format_ >= VK_FORMAT_D16_UNORM_S8_UINT)
      aspect_ |= VK_IMAGE_ASPECT_STENCIL_BIT;
    layout_ = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }
  return *this;
}

auto FramebufferAttachmentBuilder::IsValid() const -> bool {
  NOT_IMPLEMENTED(WARNING);  // TODO: implement @s0cks
  return true;
}

auto FramebufferAttachmentBuilder::Build() -> FramebufferAttachment* {
  ASSERT(IsValid());
  vk::Image* image = nullptr;
  {
    vk::ImageBuilder builder{};
    image = builder.WithType(VK_IMAGE_TYPE_2D)
                .WithFormat(GetFormat())
                .WithExtent(GetExtent())
                .WithUsage(GetUsage() | VK_IMAGE_USAGE_SAMPLED_BIT);
  }
  ASSERT_INITIALIZED(image);
  vk::ImageView* view = nullptr;
  {
    vk::ImageViewBuilder builder{};
    // clang-format off
    view = builder.WithType(VK_IMAGE_VIEW_TYPE_2D)
               .WithFormat(GetFormat())
               .WithAspectMask(GetAspectMask());
    // clang-format on
  }
  ASSERT_INITIALIZED(view);
  return new FramebufferAttachment(GetFormat(), image, view);
}
}  // namespace prt