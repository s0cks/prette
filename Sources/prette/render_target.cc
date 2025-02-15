#include "prette/render_target.h"

#include <vulkan/vulkan_core.h>

namespace prt {
RenderTarget::RenderTarget(const VkRenderPass& pass, const VkExtent2D& extent, const VkFormat format) {
  const auto driver = Driver::Get();
  ASSERT(driver);

  VkImageCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  create_info.imageType = VK_IMAGE_TYPE_2D;
  create_info.format = format;
  create_info.extent.width = extent.width;
  create_info.extent.height = extent.height;
  create_info.extent.depth = 1.0f;
  create_info.arrayLayers = 1;
  create_info.mipLevels = 1;
  create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  create_info.tiling = VK_IMAGE_TILING_LINEAR;
  create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  CHECK_VK(FATAL, vkCreateImage(driver->GetDevice(), &create_info, driver->GetAllocator(), &image_), "failed to create vk image");

  VkMemoryRequirements mem_requirements{};
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  vkGetImageMemoryRequirements(driver->GetDevice(), image_, &mem_requirements);
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = FindMemoryType(driver->GetPhysicalDevice(), mem_requirements.memoryTypeBits,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  vkAllocateMemory(driver->GetDevice(), &alloc_info, driver->GetAllocator(), &memory_);
  vkBindImageMemory(driver->GetDevice(), image_, memory_, 0);

  {
    SingleUseCommandBuffer copy;
    VkImageMemoryBarrier mem_barrier{};
    mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    mem_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    mem_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    mem_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    mem_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    mem_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    mem_barrier.image = image_;
    mem_barrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    vkCmdPipelineBarrier(copy, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &mem_barrier);
  }

  VkImageViewCreateInfo view_info{};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image_;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = format;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;
  CHECK_VK(FATAL, vkCreateImageView(driver->GetDevice(), &view_info, driver->GetAllocator(), &view_),
           "failed to create vk image view");

  VkFramebufferCreateInfo fb_info{};
  fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  fb_info.renderPass = pass;
  fb_info.attachmentCount = 1;
  fb_info.pAttachments = &view_;
  fb_info.width = extent.width;
  fb_info.height = extent.height;
  fb_info.layers = 1;
  CHECK_VK(FATAL, vkCreateFramebuffer(driver->GetDevice(), &fb_info, driver->GetAllocator(), &framebuffer_),
           "failed to create vk framebuffers");
}

void RenderTarget::Destroy() const {
  const auto driver = Driver::Get();
  ASSERT(driver);
  vkDestroyImageView(driver->GetDevice(), view_, driver->GetAllocator());
  vkDestroyImage(driver->GetDevice(), image_, driver->GetAllocator());
  vkFreeMemory(driver->GetDevice(), memory_, driver->GetAllocator());
  vkDestroyFramebuffer(driver->GetDevice(), framebuffer_, driver->GetAllocator());
}
}  // namespace prt