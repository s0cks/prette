#ifndef PRT_IMAGE_LAYOUT_TRANSITION_H
#define PRT_IMAGE_LAYOUT_TRANSITION_H

#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/image/image.h"
#include "prette/vk_cmd_buffers.h"

namespace prt::vk {
using ImageLayoutTransitionStages = std::pair<VkImageLayout, VkImageLayout>;

template <typename T>
concept ImageLayoutTransitionType = requires {
  { T::kOldLayout };
  { T::kNewLayout };
  { T::kSourceAccessMask };
  { T::kSourceStageMask };
  { T::kDestStageMask };
  { T::kDestAccessMask };
};

template <ImageLayoutTransitionType Transition>
class TransitionImageLayout {
 public:
  static constexpr const VkImageSubresourceRange kDefaultSubresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };

 public:
  TransitionImageLayout() = default;
  ~TransitionImageLayout() = default;

  void Apply(VkCommandBuffer buffer, Image* image,
             const VkImageSubresourceRange subresource_range = kDefaultSubresourceRange) {
    ASSERT_INITIALIZED(image);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = Transition::kOldLayout;
    barrier.newLayout = Transition::kNewLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = (*image);
    barrier.subresourceRange = subresource_range;
    barrier.srcAccessMask = Transition::kSourceAccessMask;
    barrier.dstAccessMask = Transition::kDestAccessMask;
    vkCmdPipelineBarrier(buffer, Transition::kSourceStageMask, Transition::kDestStageMask, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier);
  }

  void Apply(Image* image, const VkImageSubresourceRange subresource_range = kDefaultSubresourceRange) {
    ASSERT_INITIALIZED(image);
    SingleUseCommandBuffer buffer{};
    return Apply(buffer, image, subresource_range);
  }
};

template <ImageLayoutTransitionType Transition>
static inline void ApplyImageTransition(VkCommandBuffer buffer, Image* image) {
  ASSERT_INITIALIZED(image);
  vk::TransitionImageLayout<Transition> transition{};
  return transition.Apply(buffer, image);
}

template <ImageLayoutTransitionType Transition>
static inline void ApplyImageTransition(Image* image) {
  ASSERT_INITIALIZED(image);
  vk::TransitionImageLayout<Transition> transition{};
  return transition.Apply(image);
}

template <const VkImageLayout OldLayout, const VkImageLayout NewLayout>
struct ImageLayoutTransitionTemplate {
  static constexpr const auto kOldLayout = OldLayout;
  static constexpr const auto kNewLayout = NewLayout;
};

struct UndefinedToTransferDest :
  public ImageLayoutTransitionTemplate<VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL> {
  static constexpr const VkAccessFlags kSourceAccessMask = VK_ACCESS_NONE;
  static constexpr const VkAccessFlags kDestAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  static constexpr const VkPipelineStageFlags kSourceStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  static constexpr const VkPipelineStageFlags kDestStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
};

struct TransferDestToShaderReadOnly :
  public ImageLayoutTransitionTemplate<VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL> {
  static constexpr const VkAccessFlags kSourceAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  static constexpr const VkAccessFlags kDestAccessMask = VK_ACCESS_SHADER_READ_BIT;
  static constexpr const VkPipelineStageFlags kSourceStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
  static constexpr const VkPipelineStageFlags kDestStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
};
}  // namespace prt::vk

#endif  // PRT_IMAGE_LAYOUT_TRANSITION_H
