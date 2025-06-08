#include "prette/gbuffer.h"

#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/framebuffer/framebuffer.h"
#include "prette/framebuffer/framebuffer_attachment.h"
#include "prette/framebuffer/framebuffer_attachment_builder.h"
#include "prette/gfx.h"

namespace prt {
static inline auto CreateAttachment(const VkExtent2D extent, const VkFormat format, const VkImageUsageFlagBits usage)
    -> FramebufferAttachment* {
  FramebufferAttachmentBuilder builder(format);
  builder.WithExtent(extent);
  builder.WithUsage(usage);
  return builder;
}

GBuffer::GBuffer(const VkExtent2D extent, const ColorAttachmentArray color_attachments,
                 FramebufferAttachment* depth_attachment, Framebuffer* framebuffer) :
  extent_(std::move(extent)),
  depth_attachment_(depth_attachment),
  framebuffer_(framebuffer) {
  for (auto idx = 0; idx < kTotalNumberOfColorAttachments; idx++) {
    color_attachments_.at(idx) = color_attachments.at(idx);
    ASSERT_INITIALIZED(color_attachments_.at(idx));
  }
  ASSERT_INITIALIZED(depth_attachment_);
  ASSERT_INITIALIZED(framebuffer_);
}

GBuffer::~GBuffer() {
  delete framebuffer_;
  for (const auto& attachment : color_attachments_)
    delete attachment;
  delete depth_attachment_;
}

void GBuffer::InitColorAttachments() {
  color_attachments_[kPosition] =
      CreateAttachment(GetExtent(), VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ASSERT_INITIALIZED(color_attachments_[kPosition]);
  color_attachments_[kNormal] =
      CreateAttachment(GetExtent(), VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ASSERT_INITIALIZED(color_attachments_[kNormal]);
  color_attachments_[kAlbedo] =
      CreateAttachment(GetExtent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ASSERT_INITIALIZED(color_attachments_[kAlbedo]);
  color_attachments_[kMetallic] =
      CreateAttachment(GetExtent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ASSERT_INITIALIZED(color_attachments_[kMetallic]);
  color_attachments_[kRoughness] =
      CreateAttachment(GetExtent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ASSERT_INITIALIZED(color_attachments_[kRoughness]);
  color_attachments_[kAo] =
      CreateAttachment(GetExtent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
  ASSERT_INITIALIZED(color_attachments_[kAo]);
}

void GBuffer::InitDepthAttachment() {
  const auto driver = Driver::Get();
  depth_attachment_ =
      CreateAttachment(GetExtent(), driver->GetDepthFormat(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  ASSERT_INITIALIZED(depth_attachment_);
}
}  // namespace prt
