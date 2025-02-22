#include "prette/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_include.h>

#include "prette/flags.h"

namespace prt {
auto ReadTexture(const std::string& filename, const bool flip) -> TextureData {
  const auto image_path = fs::path(FLAGS_resources) / "textures" / filename;
  const int force_channels = STBI_rgb_alpha;

  int width = 0, height = 0, num_channels = 0;
  TextureBytes bytes = {stbi_load(image_path.c_str(), &width, &height, &num_channels, force_channels),
                        (void (*)(uint8_t*))stbi_image_free};
  return {.data = std::move(bytes), .width = width, .height = height, .num_channels = num_channels};
}

Texture::Texture(const Type type, std::string filename, const bool flip) :
  type_(type),
  format_(type == Type::kDiffuseMap ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM),
  mips_(1) {  // TODO: static_cast<uint32_t>(std::floor(std::log2(std::max(width_, height_)))) +
  auto textureData = ReadTexture(filename, flip);
  width_ = textureData.width;
  height_ = textureData.height;
  std::tie(image_, memory_) =
      CreateImage(width_, height_, mips_, VK_SAMPLE_COUNT_1_BIT, format_, VK_IMAGE_TILING_OPTIMAL,
                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, type == Type::kCubeMap);

  view_ = CreateImageView(image_, format_, VK_IMAGE_ASPECT_COLOR_BIT, mips_, type == Texture::kCubeMap);
  InitSampler(sampler_, mips_);

  TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mips_);
  CopyBufferToImage(textureData.data.get());
  GenerateMipmaps(image_, VK_FORMAT_R8G8B8A8_SRGB, static_cast<int32_t>(width_), static_cast<int32_t>(height_), mips_);
}

void Texture::CopyBufferToImage(VkImage image, uint32_t texWidth, uint32_t texHeight, uint8_t* data) {
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {.x = 0, .y = 0, .z = 0};
  region.imageExtent = {.width = texWidth, .height = texHeight, .depth = 1};

  const auto size = static_cast<size_t>(texWidth) * static_cast<size_t>(texHeight) * size_t{4};
  vk::Buffer::CopyDataToImageWithStaging(image, data, size, {region});
}

auto Texture::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
                          VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool cubemap)
    -> std::pair<VkImage, VkDeviceMemory> {
  VkImage image{};
  VkDeviceMemory imageMemory{};

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevels;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = numSamples;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (cubemap) {
    // Cube faces count as array layers in Vulkan
    imageInfo.arrayLayers = 6;
    // This flag is required for cube map images
    imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
  }

  const auto driver = Driver::Get();
  ASSERT(driver);
  CHECK_VK(FATAL, vkCreateImage(driver->GetDevice(), &imageInfo, nullptr, &image), "failed to create image!");

  vk::Buffer::AllocateImageMemory(driver, image, imageMemory, properties);
  vkBindImageMemory(driver->GetDevice(), image, imageMemory, 0);
  return {image, imageMemory};
}

void Texture::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
  const auto driver = Driver::Get();
  ASSERT(driver);

  // Check if image format supports linear blitting
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(driver->GetPhysicalDevice(), imageFormat, &formatProperties);

  if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    LOG(FATAL) << "Texture image format does not support linear blitting!";
  }

  SingleUseCommandBuffer buffer{};
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = image;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseMipLevel = 0;

  int32_t mipWidth = texWidth;
  int32_t mipHeight = texHeight;

  for (uint32_t i = 1; i < mipLevels; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &barrier);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                   VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr,
                         1, &barrier);

    if (mipWidth > 1) {
      mipWidth /= 2;
    }

    if (mipHeight > 1) {
      mipHeight /= 2;
    }
  }

  barrier.subresourceRange.baseMipLevel = mipLevels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr,
                       1, &barrier);
}

auto Texture::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool cubemap)
    -> VkImageView {
  const auto driver = Driver::Get();
  ASSERT(driver);
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = !cubemap ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_CUBE;
  viewInfo.format = format;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = !cubemap ? 1 : 6;
  viewInfo.subresourceRange.baseMipLevel = 0;

  VkImageView imageView{};
  CHECK_VK(FATAL, vkCreateImageView(driver->GetDevice(), &viewInfo, driver->GetAllocator(), &imageView),
           "Failed to create texture image view!");

  return imageView;
}

void Texture::InitSampler(VkSampler& sampler, const uint32_t mips) {
  const auto driver = Driver::Get();
  ASSERT(driver);
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(driver->GetPhysicalDevice(), &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = static_cast<float>(mips);
  samplerInfo.mipLodBias = 0.0f;
  CHECK_VK(FATAL, vkCreateSampler(driver->GetDevice(), &samplerInfo, driver->GetAllocator(), &sampler),
           "Failed to create texture sampler!");
}

void Texture::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels,
                                    bool cubemap) {
  SingleUseCommandBuffer buffer{};
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mipLevels;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = !cubemap ? 1 : 6;

  VkPipelineStageFlags sourceStage = {};
  VkPipelineStageFlags destinationStage = {};

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    LOG(FATAL) << "invalid layout transition";
  }

  vkCmdPipelineBarrier(buffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Texture::Destroy() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  vkDestroySampler(driver->GetDevice(), sampler_, driver->GetAllocator());
  vkDestroyImageView(driver->GetDevice(), view_, driver->GetAllocator());
  vkDestroyImage(driver->GetDevice(), image_, driver->GetAllocator());
  vkFreeMemory(driver->GetDevice(), memory_, driver->GetAllocator());
}
}  // namespace prt