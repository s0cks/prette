#include "prette/image/image.h"

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/alloc_mem_req.h"
#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/image/image_builder.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"
#include "prette/vk_cmd_buffers.h"

namespace prt::vk {
Image::Image(const VkImageCreateInfo* create_info, const VkMemoryPropertyFlags mem_flags) :
  vk::HandleTemplate<VkImage>() {
  const auto driver = Driver::Get();
  driver->CreateImage(create_info, handle_ptr());

  vk::AllocMemoryRequest alloc(handle_ref(), mem_flags);
  alloc.Allocate(&memory_);
}

Image::~Image() {
  const auto driver = Driver::Get();
  driver->DestroyImage(handle_ref());
  driver->Destroy(memory_);
}

void ImageMemoryBarrier::operator()(VkCommandBuffer buffer, const VkPipelineStageFlags source_stage,
                                    const VkPipelineStageFlags dest_stage) {
  ASSERT(buffer != VK_NULL_HANDLE);
  return vkCmdPipelineBarrier(buffer, source_stage, dest_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier_);
}

auto Image::TransitionLayout(const VkImageLayout old_layout, const VkImageLayout new_layout, uint32_t mips,
                             bool cubemap) -> bool {
  vk::SingleUseCommandBuffer buffer{};

  ImageMemoryBarrier barrier(this);
  barrier.WithOldLayout(old_layout)
      .WithNewLayout(new_layout)
      .WithSubresourceRange(VkImageSubresourceRange{
          VK_IMAGE_ASPECT_COLOR_BIT,
          0,
          mips,
          0,
          !cubemap ? 1 : 6,
      });

  VkPipelineStageFlags source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags dest_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

  switch (old_layout) {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      barrier.WithSourceAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);
      source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      barrier.WithSourceAccessMask(VK_ACCESS_SHADER_READ_BIT);
      source_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      break;
    default:
      LOG(FATAL) << "invalid image transition source layout: " << old_layout;
  }

  switch (new_layout) {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      barrier.WithDestAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);
      dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      barrier.WithSourceAccessMask(VK_ACCESS_TRANSFER_WRITE_BIT);
      barrier.WithDestAccessMask(VK_ACCESS_SHADER_READ_BIT);
      dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      break;
    default:
      LOG(FATAL) << "invalid image transition dest layout: " << new_layout;
  }

  barrier(buffer, source_stage, dest_stage);
  return true;
}

void Image::BindMemory(const VkDeviceSize offset) {
  ASSERT(HasMemory());
  ASSERT(offset >= 0);
  const auto driver = Driver::Get();
  vkBindImageMemory(*driver->GetDevice(), handle_ref(), memory_, offset);
}

auto Image::ToString() const -> std::string {
  return ToStringHelper<Image>{};
}

auto NewDepthImage(const VkExtent2D& extent, const VkFormat format) -> Image* {
  vk::ImageBuilder builder{};
  // clang-format off
  return builder.WithExtent(extent)
      .WithFormat(format)
      .WithOptimalTiling()
      .WithDepthStencilAttachmentUsage();
  // clang-format on
}
}  // namespace prt::vk