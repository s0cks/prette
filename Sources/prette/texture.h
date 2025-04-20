#ifndef PRT_TEXTURE_H
#define PRT_TEXTURE_H

#include <vulkan/vulkan_core.h>

#include "prette/gfx.h"

namespace prt {
using TextureBytes = std::unique_ptr<uint8_t[], std::function<void(uint8_t*)>>;

struct TextureData {
  TextureBytes data;
  int width;
  int height;
  int num_channels;
};

auto ReadTexture(const std::string& filename, const bool flip) -> TextureData;

class Texture {
 public:
  enum Type {
    kDiffuseMap,
    kSpecularMap,
    kNormalMap,
    kCubeMap,
  };

 private:
  std::string name_ = "default_texture_name";
  Type type_;
  VkImage image_ = {};
  VkDeviceMemory memory_ = {};
  VkImageView view_ = {};
  VkSampler sampler_ = {};
  VkFormat format_ = {};
  uint32_t mips_ = {};
  uint32_t channels_ = {};
  uint32_t width_ = {};
  uint32_t height_ = {};

  inline void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    TransitionImageLayout(image_, oldLayout, newLayout, mipLevels);
  }

 public:
  Texture(const Type type, std::string filename, const bool flip = false);
  Texture() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto GetType() const -> Type {
    return type_;
  }

  auto GetImage() const -> VkImage const& {
    return image_;
  }

  auto GetImageView() const -> VkImageView const& {
    return view_;
  }

  auto GetSampler() const -> VkSampler const& {
    return sampler_;
  }

  auto GetFormat() const -> VkFormat const& {
    return format_;
  }

  auto GetMipLevel() const -> uint32_t {
    return mips_;
  }

  auto GetNumberOfChannels() const -> uint32_t {
    return channels_;
  }

  auto GetWidth() const -> uint32_t {
    return width_;
  }

  auto GetHeight() const -> uint32_t {
    return height_;
  }

  void Destroy();
  void CopyBufferToImage(uint8_t* data) {
    return CopyBufferToImage(image_, width_, height_, data);
  }

 public:
  static auto CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
                          VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool cubemap = false)
      -> std::pair<VkImage, VkDeviceMemory>;
  static void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
  static auto CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              bool cubemap = false) -> VkImageView;
  static void InitSampler(VkSampler& sampler, const uint32_t mips);

  static void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels,
                                    bool cubemap = false);
  static void CopyBufferToCubemapImage(VkImage image, uint32_t texWidth, uint32_t texHeight, uint8_t* data);
  static void CopyBufferToImage(VkImage image, uint32_t texWidth, uint32_t texHeight, uint8_t* data);
};
}  // namespace prt

#endif  // PRT_TEXTURE_H
