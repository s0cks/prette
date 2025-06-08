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
    kNormal,
    kAlbedo,
    kMetallic,
    kRoughness,
    kAo,
    kTotalNumberOfColorAttachments,
  };

  using ColorAttachmentArray = std::array<FramebufferAttachment*, kTotalNumberOfColorAttachments>;

  static constexpr const auto kPosFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
  static constexpr const auto kNormalFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
  static constexpr const auto kAlbedoFormat = VK_FORMAT_R8G8B8A8_UNORM;
  static constexpr const auto kMetallicFormat = VK_FORMAT_R8G8B8A8_UNORM;
  static constexpr const auto kRoughnessFormat = VK_FORMAT_R8G8B8A8_UNORM;
  static constexpr const auto kAoFormat = VK_FORMAT_R8G8B8A8_UNORM;

 private:
  VkExtent2D extent_{};
  ColorAttachmentArray color_attachments_{};
  FramebufferAttachment* depth_attachment_ = nullptr;
  Framebuffer* framebuffer_ = nullptr;

  void InitColorAttachments();
  void InitDepthAttachment();

 public:
  explicit GBuffer(const VkExtent2D extent, const ColorAttachmentArray color_attachments,
                   FramebufferAttachment* depth_attachment, Framebuffer* framebuffer);
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

  inline auto GetPositionAttachment() const -> FramebufferAttachment* {
    return color_attachments_[kPosition];
  }

  inline auto GetNormalAttachment() const -> FramebufferAttachment* {
    return color_attachments_[kNormal];
  }

  inline auto GetAlbedoTexture() const -> FramebufferAttachment* {
    return color_attachments_[kAlbedo];
  }

  inline auto GetMetallicTexture() const -> FramebufferAttachment* {
    return color_attachments_[kMetallic];
  }

  inline auto GetRoughnessTexture() const -> FramebufferAttachment* {
    return color_attachments_[kRoughness];
  }

  inline auto GetAoTexture() const -> FramebufferAttachment* {
    return color_attachments_[kAo];
  }
};
}  // namespace prt

#endif  // PRT_GBUFFER_H
