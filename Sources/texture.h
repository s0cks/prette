#ifndef PRT_TEXTURE_H
#define PRT_TEXTURE_H

#include "prette/gfx.h"

namespace prt {
using TextureBytes = std::unique_ptr<uint8_t[], std::function<void(uint8_t*)>>;

struct TextureData {
  TextureBytes data;
  int width;
  int height;
  int num_channels;
};

auto ReadTexture(const std::string& filename, const bool flip = false) -> TextureData;

class Texture {
 public:
  enum Type {
    kDiffuseMap,
    kSpecularMap,
    kNormalMap,
    kCubeMap,
  };

 private:
  Type type_;
  VkImage m_textureImage = {};
  VkDeviceMemory m_textureImageMemory = {};
  VkImageView m_textureImageView = {};
  VkSampler m_textureSampler = {};
  VkFormat m_format = {};
  // int32_t m_channels = {};
  uint32_t m_mips = {};
  uint32_t m_width = {};
  uint32_t m_height = {};
  std::string m_name = "default_texture_name";

  inline void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    TransitionImageLayout(m_textureImage, oldLayout, newLayout, mipLevels);
  }

 public:
  Texture(const Type type, std::string_view textureName);

  Texture() = default;

  void Destroy();

  void CreateTextureImage(Type type, std::string textureName);

  auto GetImageViewAndSampler() const -> std::pair<VkImageView, VkSampler> {
    return {m_textureImageView, m_textureSampler};
  }

  void CreateTextureSampler() {
    m_textureSampler = CreateSampler(m_mips);
  }

  auto GetType() const -> Type {
    return type_;
  }

  void CopyBufferToImage(uint8_t* data) {
    return CopyBufferToImage(m_textureImage, m_width, m_height, data);
  }

  static auto CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
                          VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool cubemap = false)
      -> std::pair<VkImage, VkDeviceMemory>;
  static void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
  static auto CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              bool cubemap = false) -> VkImageView;
  static auto CreateSampler(uint32_t mipLevels = 1) -> VkSampler;
  static void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels,
                                    bool cubemap = false);
  static void CopyBufferToCubemapImage(VkImage image, uint32_t texWidth, uint32_t texHeight, uint8_t* data);
  static void CopyBufferToImage(VkImage image, uint32_t texWidth, uint32_t texHeight, uint8_t* data);
};
}  // namespace prt

#endif  // PRT_TEXTURE_H
