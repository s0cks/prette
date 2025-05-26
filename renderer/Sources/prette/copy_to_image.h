#ifndef PRT_COPY_TO_IMAGE_H
#define PRT_COPY_TO_IMAGE_H

#include <cstdint>

#include "prette/image/image_layout_transition.h"
#include "prette/vk.h"

namespace prt::vk {
class CopyBufferToImage {
 private:
  VkBufferImageCopy region_{};

  inline auto region_ptr() -> VkBufferImageCopy* {
    return &region_;
  }

  inline auto region() const -> const VkBufferImageCopy& {
    return region_;
  }

 public:
  CopyBufferToImage() {
    region_ptr()->bufferOffset = 0;
    region_ptr()->bufferRowLength = 0;
    region_ptr()->bufferImageHeight = 0;
    region_ptr()->imageSubresource = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    region_ptr()->imageOffset = {};
    region_ptr()->imageExtent = {};
  }
  explicit CopyBufferToImage(const VkRect2D bounds) :
    CopyBufferToImage() {
    region_ptr()->imageExtent = VkExtent2DTo3D(bounds.extent);
    region_ptr()->imageOffset = VkOffset2DTo3D(bounds.offset);
  }
  virtual ~CopyBufferToImage() = default;

  auto WithBufferOffset(const int rhs) -> CopyBufferToImage& {
    region_ptr()->bufferOffset = rhs;
    return *this;
  }

  auto WithBufferRowLength(const int rhs) -> CopyBufferToImage& {
    region_ptr()->bufferRowLength = rhs;
    return *this;
  }

  auto WithBufferImageHeight(const int rhs) -> CopyBufferToImage& {
    region_ptr()->bufferImageHeight = rhs;
    return *this;
  }

  auto WithVkImageSubresourceLayers(const VkImageSubresourceLayers rhs) -> CopyBufferToImage& {
    region_ptr()->imageSubresource = rhs;
    return *this;
  }

  auto WithOffset(const VkOffset3D rhs) -> CopyBufferToImage& {
    region_ptr()->imageOffset = rhs;
    return *this;
  }

  inline auto WithOffset(const VkOffset2D rhs) -> CopyBufferToImage& {
    return WithOffset(VkOffset3D{
        .x = rhs.x,
        .y = rhs.y,
        .z = 0,
    });
  }

  auto WithExtent(const VkExtent3D rhs) -> CopyBufferToImage& {
    region_ptr()->imageExtent = rhs;
    return *this;
  }

  inline auto WithExtent(const VkExtent2D rhs, const uint32_t depth = 1) -> CopyBufferToImage& {
    return WithExtent(VkExtent3D{
        .width = rhs.width,
        .height = rhs.height,
        .depth = depth,
    });
  }

  auto WithBounds(const VkRect2D rhs) -> CopyBufferToImage& {
    region_ptr()->imageExtent = VkExtent2DTo3D(rhs.extent);
    region_ptr()->imageOffset = VkOffset2DTo3D(rhs.offset);
    return *this;
  }

  virtual void Apply(Buffer* src, Image* dst, const VkImageLayout dst_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  void operator()(Buffer* src, Image* dst, const VkImageLayout dst_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    return Apply(src, dst, dst_layout);
  }
};

template <ImageLayoutTransitionType PreTransition, ImageLayoutTransitionType PostTransition>
class CopyBufferToImageWithTransitions : public CopyBufferToImage {
 public:
  CopyBufferToImageWithTransitions() = default;
  explicit CopyBufferToImageWithTransitions(const VkRect2D bounds) :
    CopyBufferToImage(bounds) {}
  ~CopyBufferToImageWithTransitions() override = default;

  void Apply(Buffer* src, Image* dst, const VkImageLayout dst_layout = PostTransition::kNewLayout) override {
    vk::ApplyImageTransition<PreTransition>(dst);
    CopyBufferToImage::Apply(src, dst, dst_layout);
    vk::ApplyImageTransition<PostTransition>(dst);
  }

  void operator()(Buffer* src, Image* dst, const VkImageLayout dst_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    return Apply(src, dst, dst_layout);
  }
};
}  // namespace prt::vk

#endif  // PRT_COPY_TO_IMAGE_H
