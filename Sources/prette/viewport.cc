#include "prette/viewport.h"

#include <vulkan/vulkan_core.h>

#include <vector>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
static std::vector<VkImage> images_{};
static std::vector<VkDeviceMemory> memory_{};
static std::vector<VkImageView> views_{};
static VkRenderPass pass_{};
static VkPipeline pipeline_{};
static VkCommandPool pool_{};
static std::vector<VkFramebuffer> framebuffers_{};
static std::vector<VkCommandBuffer> command_buffers_{};

// static inline void InitRenderPass(const Driver* driver) {
//   VkAttachmentDescription color_att{};
//   color_att.format = color_format;
//   color_att.samples = VK_SAMPLE_COUNT_1_BIT;
//   color_att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//   color_att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//   color_att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//   color_att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   color_att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//   color_att.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

//   VkAttachmentDescription depth_att{};
//   depth_att.format = depth_format;
//   depth_att.samples = VK_SAMPLE_COUNT_1_BIT;
//   depth_att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//   depth_att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//   depth_att.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//   depth_att.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   depth_att.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//   depth_att.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

//   std::array<VkAttachmentDescription, 2> attachments = {};
// }

static inline void InitImages(const Driver* driver, const int num_images, const VkExtent3D& extent) {
  images_.resize(num_images);
  for (auto idx = 0; idx < num_images; idx++) {
    VkImageCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = VK_IMAGE_TYPE_2D;
    create_info.format = VK_FORMAT_B8G8R8A8_SRGB;
    create_info.extent.width = extent.width;
    create_info.extent.height = extent.height;
    create_info.extent.depth = extent.depth;
    create_info.arrayLayers = 1;
    create_info.mipLevels = 1;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    create_info.tiling = VK_IMAGE_TILING_LINEAR;
    create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    CHECK_VK(FATAL, vkCreateImage(driver->GetDevice(), &create_info, driver->GetAllocator(), &images_[idx]),
             "failed to create vk image");

    VkMemoryRequirements mem_requirements{};
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkGetImageMemoryRequirements(driver->GetDevice(), images_[idx], &mem_requirements);

    alloc_info.memoryTypeIndex = FindMemoryType(driver->GetPhysicalDevice(), mem_requirements.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    CHECK_VK(FATAL, vkAllocateMemory(driver->GetDevice(), &alloc_info, driver->GetAllocator(), &memory_[idx]),
             "failed to create vk image memory");
    CHECK_VK(FATAL, vkBindImageMemory(driver->GetDevice(), images_[idx], memory_[idx], 0), "failed to bind vk image memory");

    SingleUseCommandBuffer buffer(pool_);
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.image = images_[idx];
    barrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier);
  }
}

static inline void InitImageViews(const Driver* driver, const int num_views) {
  ASSERT(driver);
  ASSERT(num_views >= 1);
  views_.resize(num_views);
  for (auto idx = 0; idx < num_views; idx++) {
    views_[idx] = vk::NewImageView(driver, images_[idx], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
  }
}

// static inline void InitFramebuffers(const Driver* driver, const int num_framebuffers) {
//   ASSERT(driver);
//   ASSERT(num_framebuffers >= 1);
//   framebuffers_.resize(num_framebuffers);
//   for (auto idx = 0; idx < num_framebuffers; idx++) {
//     std::array<VkImageView, 2> attachments = {views_[idx], depth_};

//     VkFramebufferCreateInfo create_info{};
//     create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//     create_info.renderPass = pass_;
//     create_info.attachmentCount = attachments.size();
//     create_info.pAttachments = &attachments[0];
//     create_info.width = width;
//     create_info.height = height;
//     create_info.layers = 1;
//     CHECK_VK(FATAL, vkCreateFramebuffer(driver->GetDevice(), &create_info, driver->GetAllocator(), &framebuffers_[idx]),
//              "failed to create viewport vk framebuffer");
//   }
// }

static inline void InitCommandBuffers(const Driver* driver, const int num_buffers) {
  ASSERT(driver);
  ASSERT(num_buffers >= 1);
  command_buffers_.resize(num_buffers);
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = num_buffers;
  CHECK_VK(FATAL, vkAllocateCommandBuffers(driver->GetDevice(), &alloc_info, command_buffers_.data()),
           "failed to allocate vk command buffers");
}
}  // namespace prt