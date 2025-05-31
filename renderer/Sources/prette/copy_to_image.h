#ifndef PRT_COPY_TO_IMAGE_H
#define PRT_COPY_TO_IMAGE_H

#include <cstdint>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/image/image_layout_transition.h"
#include "prette/vk.h"

namespace prt::vk {
class CopyBufferToImage {
 public:
  static constexpr const auto kDefaultDestLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

 private:
  Buffer* source_;
  VkBufferImageCopy region_{};

  inline auto region_ptr() -> VkBufferImageCopy* {
    return &region_;
  }

  inline auto region() const -> const VkBufferImageCopy& {
    return region_;
  }

 public:
  explicit CopyBufferToImage(Buffer* source);
  CopyBufferToImage(Buffer* source, const VkRect2D bounds) :
    CopyBufferToImage(source) {
    region_ptr()->imageExtent = VkExtent2DTo3D(bounds.extent);
    region_ptr()->imageOffset = VkOffset2DTo3D(bounds.offset);
  }
  virtual ~CopyBufferToImage() = default;

  auto GetSource() const -> Buffer* {
    return source_;
  }

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

  inline auto WithOffset(const VkOffset2D rhs, const int32_t z = 0) -> CopyBufferToImage& {
    return WithOffset(VkOffset3D{
        .x = rhs.x,
        .y = rhs.y,
        .z = z,
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
    WithExtent(rhs.extent);
    WithOffset(rhs.offset);
    return *this;
  }

  virtual void Apply(Image* dst, const VkImageLayout dst_layout = kDefaultDestLayout);

  void operator()(Image* dst, const VkImageLayout dst_layout = kDefaultDestLayout) {
    return Apply(dst, dst_layout);
  }

 public:
  static inline void Copy(Buffer* source, const VkRect2D bounds, Image* dest,
                          const VkImageLayout dest_layout = kDefaultDestLayout) {
    CopyBufferToImage copy(source, bounds);
    copy(dest, dest_layout);
  }

  static inline void Copy(Buffer* source, Image* dest, const VkImageLayout dest_layout = kDefaultDestLayout) {
    CopyBufferToImage copy(source);
    copy(dest, dest_layout);
  }
};

class CopyBytesToImageWithStaging {
 public:
  static constexpr const auto kDefaultDestLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

 private:
  VkRect2D bounds_{};
  const uint8_t* bytes_;
  uint64_t num_bytes_;

 public:
  CopyBytesToImageWithStaging(const uint8_t* bytes, const uint64_t num_bytes, const VkRect2D bounds) :
    bounds_(std::move(bounds)),
    bytes_(bytes),
    num_bytes_(num_bytes) {
    ASSERT(bytes);
    ASSERT_GT(num_bytes, 0);
  }
  CopyBytesToImageWithStaging(const uint8_t* bytes, const uint64_t num_bytes) :
    bytes_(bytes),
    num_bytes_(num_bytes) {
    ASSERT(bytes);
    ASSERT_GT(num_bytes, 0);
  }
  virtual ~CopyBytesToImageWithStaging() = default;

  auto GetSourceBytes() const -> const uint8_t* {
    return bytes_;
  }

  auto GetNumberOfSourceBytes() const -> uint64_t {
    return num_bytes_;
  }

  virtual void Apply(Image* dest, const VkImageLayout dst_layout = kDefaultDestLayout);

  void operator()(Image* dest, const VkImageLayout dest_layout = kDefaultDestLayout) {
    return Apply(dest, dest_layout);
  }

 public:
  static inline void Copy(const uint8_t* bytes, const uint64_t num_bytes, const VkRect2D bounds, Image* dest,
                          const VkImageLayout dest_layout = kDefaultDestLayout) {
    CopyBytesToImageWithStaging copy(bytes, num_bytes, bounds);
    copy(dest, dest_layout);
  }

  static inline void Copy(const uint8_t* bytes, const uint64_t num_bytes, Image* dest,
                          const VkImageLayout dest_layout = kDefaultDestLayout) {
    CopyBytesToImageWithStaging copy(bytes, num_bytes);
    copy(dest, dest_layout);
  }
};

template <ImageLayoutTransitionType PreTransition, ImageLayoutTransitionType PostTransition>
class CopyBufferToImageWithTransitions : public CopyBufferToImage {
 public:
  CopyBufferToImageWithTransitions() = default;
  CopyBufferToImageWithTransitions(Buffer* source, const VkRect2D bounds) :
    CopyBufferToImage(source, bounds) {}
  explicit CopyBufferToImageWithTransitions(Buffer* source) :
    CopyBufferToImage(source) {}
  ~CopyBufferToImageWithTransitions() override = default;

  void Apply(Image* dst, const VkImageLayout dst_layout = PostTransition::kNewLayout) override {
    vk::ApplyImageTransition<PreTransition>(dst);
    CopyBufferToImage::Apply(dst, dst_layout);
    vk::ApplyImageTransition<PostTransition>(dst);
  }

  void operator()(Image* dst, const VkImageLayout dst_layout = kDefaultDestLayout) {
    return Apply(dst, dst_layout);
  }

 public:
  static inline void Copy(Buffer* source, const VkRect2D bounds, Image* dest,
                          const VkImageLayout dest_layout = kDefaultDestLayout) {
    CopyBufferToImageWithTransitions<PreTransition, PostTransition> copy(source, bounds);
    copy(dest, dest_layout);
  }

  static inline void Copy(Buffer* source, Image* dest, const VkImageLayout dest_layout = kDefaultDestLayout) {
    CopyBufferToImageWithTransitions<PreTransition, PostTransition> copy(source);
    copy(dest, dest_layout);
  }
};

template <ImageLayoutTransitionType PreTransition, ImageLayoutTransitionType PostTransition>
class CopyBytesToImageWithStagingAndTransitions : public CopyBytesToImageWithStaging {
 public:
  CopyBytesToImageWithStagingAndTransitions() = default;
  CopyBytesToImageWithStagingAndTransitions(const uint8_t* bytes, const uint64_t num_bytes, const VkRect2D bounds) :
    CopyBytesToImageWithStaging(bytes, num_bytes, bounds) {}
  CopyBytesToImageWithStagingAndTransitions(const uint8_t* bytes, const uint64_t num_bytes) :
    CopyBytesToImageWithStaging(bytes, num_bytes) {}
  ~CopyBytesToImageWithStagingAndTransitions() override = default;

  void Apply(Image* dest, const VkImageLayout dst_layout = PostTransition::kNewLayout) override {
    ASSERT_INITIALIZED(dest);
    vk::ApplyImageTransition<PreTransition>(dest);
    CopyBytesToImageWithStaging::Apply(dest, dst_layout);
    vk::ApplyImageTransition<PostTransition>(dest);
  }

  void operator()(Image* dest, const VkImageLayout dst_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    return Apply(dest, dst_layout);
  }

 public:
  static inline void Copy(const uint8_t* bytes, const uint64_t num_bytes, const VkRect2D bounds, Image* dest,
                          const VkImageLayout dest_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    CopyBytesToImageWithStagingAndTransitions<PreTransition, PostTransition> copy(bytes, num_bytes, bounds);
    copy(dest, dest_layout);
  }

  static inline void Copy(const uint8_t* bytes, const uint64_t num_bytes, const Image* dest,
                          const VkImageLayout dest_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    CopyBytesToImageWithStagingAndTransitions<PreTransition, PostTransition> copy(bytes, num_bytes);
    copy(dest, dest_layout);
  }
};
}  // namespace prt::vk

#endif  // PRT_COPY_TO_IMAGE_H
