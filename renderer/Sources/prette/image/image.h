#ifndef PRT_IMAGE_H
#define PRT_IMAGE_H

#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/platform.h"
#include "prette/vk.h"

namespace prt::vk {
class Image : public HandleTemplate<VkImage> {
 private:
  VkDeviceMemory memory_ = VK_NULL_HANDLE;

 public:
  Image(const VkImageCreateInfo* create_info,
        const VkMemoryPropertyFlags mem_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  ~Image() override;
  auto TransitionLayout(const VkImageLayout old_layout, const VkImageLayout new_layout, uint32_t mips = 1,
                        bool cubemap = false) -> bool;
  void CopyBuffer(Buffer* buffer, const VkExtent2D extent, const VkOffset2D offset);

  auto GetMemory() const -> const VkDeviceMemory& {
    return memory_;
  }

  inline auto HasMemory() const -> bool {
    return GetMemory() != VK_NULL_HANDLE;
  }

  void BindMemory(const VkDeviceSize offset = 0);
  auto ToString() const -> std::string override;

  operator VkImage() const {
    return GetHandle();
  }
};

auto NewDepthImage(const VkExtent2D& extent, const VkFormat format) -> Image*;

class ImageMemoryBarrier {
 private:
  VkImageMemoryBarrier barrier_{};

  inline auto barrier_ptr() -> VkImageMemoryBarrier* {
    return &barrier_;
  }

 public:
  ImageMemoryBarrier(Image* image) {
    ASSERT(image);
    barrier_ptr()->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier_ptr()->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_ptr()->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_ptr()->srcAccessMask = VK_ACCESS_NONE;
    barrier_ptr()->dstAccessMask = VK_ACCESS_NONE;
    barrier_ptr()->oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier_ptr()->newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier_ptr()->image = image->GetHandle();
    barrier_ptr()->subresourceRange = VkImageSubresourceRange{
        VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1,
    };
  }
  ~ImageMemoryBarrier() = default;

  operator VkImageMemoryBarrier() const {
    return barrier_;
  }

  auto WithSourceAccessMask(const VkAccessFlagBits rhs) -> ImageMemoryBarrier& {
    barrier_ptr()->srcAccessMask = rhs;
    return *this;
  }

  auto WithSourceQueueFamilyIndex(const uint32_t rhs) -> ImageMemoryBarrier& {
    barrier_ptr()->srcQueueFamilyIndex = rhs;
    return *this;
  }

  inline auto WithSourceQueueFamilyIndexIgnored() -> ImageMemoryBarrier& {
    return WithSourceQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
  }

  auto WithDestQueueFamilyIndex(const uint32_t rhs) -> ImageMemoryBarrier& {
    barrier_ptr()->dstQueueFamilyIndex = rhs;
    return *this;
  }

  inline auto WithDestQueueFamilyIndexIgnored() -> ImageMemoryBarrier& {
    return WithDestQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
  }

  auto WithDestAccessMask(const VkAccessFlagBits rhs) -> ImageMemoryBarrier& {
    barrier_ptr()->dstAccessMask = rhs;
    return *this;
  }

  auto WithOldLayout(const VkImageLayout rhs) -> ImageMemoryBarrier& {
    barrier_ptr()->oldLayout = rhs;
    return *this;
  }

  auto WithNewLayout(const VkImageLayout rhs) -> ImageMemoryBarrier& {
    barrier_ptr()->newLayout = rhs;
    return *this;
  }

  auto WithImage(const VkImage rhs) -> ImageMemoryBarrier& {
    ASSERT(rhs != VK_NULL_HANDLE);
    barrier_ptr()->image = rhs;
    return *this;
  }

  auto WithImage(Image* rhs) -> ImageMemoryBarrier& {
    ASSERT(rhs);
    return WithImage(rhs->GetHandle());
  }

  auto WithSubresourceRange(const VkImageSubresourceRange rhs) -> ImageMemoryBarrier& {
    barrier_ptr()->subresourceRange = std::move(rhs);
    return *this;
  }

  void operator()(VkCommandBuffer buffer, const VkPipelineStageFlags source_stage,
                  const VkPipelineStageFlags dest_stage);
};

class PipelineBarrier {
 private:
  std::vector<VkImageMemoryBarrier> image_barriers_{};
  VkMemoryPropertyFlags src_stage_mask_ = VK_PIPELINE_STAGE_NONE;
  VkMemoryPropertyFlags dst_stage_mask_ = VK_PIPELINE_STAGE_NONE;

 public:
  PipelineBarrier() = default;
  ~PipelineBarrier() = default;

  auto WithImageBarrier(VkImageMemoryBarrier rhs) -> PipelineBarrier& {
    image_barriers_.push_back(rhs);
    return *this;
  }

  auto WithImageBarrier(ImageMemoryBarrier* rhs) -> PipelineBarrier& {
    ASSERT(rhs);
    return WithImageBarrier(*rhs);
  }

  auto WithSourceStageMask(const VkMemoryPropertyFlags rhs) -> PipelineBarrier& {
    src_stage_mask_ = rhs;
    return *this;
  }

  auto WithDestStageMask(const VkMemoryPropertyFlags rhs) -> PipelineBarrier& {
    dst_stage_mask_ = rhs;
    return *this;
  }

  auto WithStageMask(const VkMemoryPropertyFlags rhs) -> PipelineBarrier& {
    src_stage_mask_ = rhs;
    dst_stage_mask_ = rhs;
    return *this;
  }

  void Apply(VkCommandBuffer& buffer) {
    vkCmdPipelineBarrier(buffer, src_stage_mask_, dst_stage_mask_, 0, 0, nullptr, 0, nullptr, image_barriers_.size(),
                         image_barriers_.data());
  }
};

template <const VkImageLayout OldLayout, const VkImageLayout NewLayout>
class ImageLayoutScope {
 private:
  Image* image_;

 public:
  explicit ImageLayoutScope(Image* image) :
    image_(image) {
    ASSERT_INITIALIZED(image_);
    image_->TransitionLayout(OldLayout, NewLayout);
  }
  virtual ~ImageLayoutScope() {
    ASSERT_INITIALIZED(image_);
    image_->TransitionLayout(NewLayout, OldLayout);
  }
};

template <const VkImageLayout OldLayout>
class ImageLayoutTransferDestScope : public ImageLayoutScope<OldLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL> {
 public:
  explicit ImageLayoutTransferDestScope(Image* image) :
    ImageLayoutScope<OldLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL>(image) {}
  ~ImageLayoutTransferDestScope() override = default;
};

using ReadOnlyShaderImageTransferDestScope = ImageLayoutTransferDestScope<VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL>;

template <const VkImageLayout OldLayout>
class ImageLayoutTransferSourceScope : public ImageLayoutScope<OldLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL> {
 public:
  explicit ImageLayoutTransferSourceScope(Image* image) :
    ImageLayoutScope<OldLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL>(image) {}
  ~ImageLayoutTransferSourceScope() override = default;
};
}  // namespace prt::vk

#endif  // PRT_IMAGE_H
