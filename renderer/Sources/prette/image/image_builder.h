#ifndef PRT_IMAGE_BUILDER_H
#define PRT_IMAGE_BUILDER_H

#include <cstdint>
#include <utility>

#include "prette/vk.h"

namespace prt::vk {
class ImageBuilder : public HandleBuilderTemplate<VkImageCreateInfo, Image> {
 private:
  VkMemoryPropertyFlags mem_flags_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

 public:
  ImageBuilder();
  ~ImageBuilder() override = default;

  auto WithType(const VkImageType rhs) -> ImageBuilder& {
    info_ptr()->imageType = rhs;
    return *this;
  }

  auto WithWidth(const int rhs) -> ImageBuilder& {
    info_ptr()->extent.width = rhs;
    return *this;
  }

  auto WithHeight(const int rhs) -> ImageBuilder& {
    info_ptr()->extent.height = rhs;
    return *this;
  }

  auto WithDepth(const int rhs) -> ImageBuilder& {
    info_ptr()->extent.depth = rhs;
    return *this;
  }

  auto WithMipLevels(const uint32_t rhs) -> ImageBuilder& {
    info_ptr()->mipLevels = rhs;
    return *this;
  }

  auto WithTiling(const VkImageTiling rhs) -> ImageBuilder& {
    info_ptr()->tiling = rhs;
    return *this;
  }

  inline auto WithLinearTiling() -> ImageBuilder& {
    return WithTiling(VK_IMAGE_TILING_LINEAR);
  }

  inline auto WithOptimalTiling() -> ImageBuilder& {
    return WithTiling(VK_IMAGE_TILING_OPTIMAL);
  }

  auto WithArrayLayers(const uint32_t rhs) -> ImageBuilder& {
    info_ptr()->arrayLayers = rhs;
    return *this;
  }

  auto WithFormat(const VkFormat rhs) -> ImageBuilder& {
    info_ptr()->format = rhs;
    return *this;
  }

  auto WithExtent(const VkExtent3D rhs) -> ImageBuilder& {
    info_ptr()->extent = std::move(rhs);
    return *this;
  }

  inline auto WithExtent(const VkExtent2D extent, const uint32_t depth = 1) -> ImageBuilder& {
    return WithExtent(VkExtent3D{
        .width = extent.width,
        .height = extent.height,
        .depth = depth,
    });
  }

  auto WithInitialLayout(const VkImageLayout rhs) -> ImageBuilder& {
    info_ptr()->initialLayout = rhs;
    return *this;
  }

  auto GetUsage() const -> VkImageUsageFlags {
    return info().usage;
  }

  auto WithUsage(const VkImageUsageFlags rhs) -> ImageBuilder& {
    info_ptr()->usage = rhs;
    return *this;
  }

  inline auto WithSampledUsage() -> ImageBuilder& {
    return WithUsage(GetUsage() | VK_IMAGE_USAGE_SAMPLED_BIT);
  }

  inline auto WithTransferDestUsage() -> ImageBuilder& {
    return WithUsage(GetUsage() | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  }

  inline auto WithTransferSourceUsage() -> ImageBuilder& {
    return WithUsage(GetUsage() | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  }

  inline auto WithDepthStencilAttachmentUsage() -> ImageBuilder& {
    return WithUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

  auto WithSamples(const VkSampleCountFlagBits rhs) -> ImageBuilder& {
    info_ptr()->samples = rhs;
    return *this;
  }

  auto WithSharingMode(const VkSharingMode rhs) -> ImageBuilder& {
    info_ptr()->sharingMode = rhs;
    return *this;
  }

  inline auto WithExclusiveSharingMode() -> ImageBuilder& {
    return WithSharingMode(VK_SHARING_MODE_EXCLUSIVE);
  }

  inline auto WithConcurrentSharingMode() -> ImageBuilder& {
    return WithSharingMode(VK_SHARING_MODE_CONCURRENT);
  }

  auto AsCubeMap() -> ImageBuilder& {
    info_ptr()->arrayLayers = 6;
    info_ptr()->flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    return *this;
  }

  auto WithMemoryFlags(const VkMemoryPropertyFlags rhs) -> ImageBuilder& {
    mem_flags_ = rhs;
    return *this;
  }

  auto IsValid() const -> bool override;
  auto Build() -> Image* override;

  auto operator()() -> Image* {
    return Build();
  }

  operator Image*() {
    return Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_IMAGE_BUILDER_H
