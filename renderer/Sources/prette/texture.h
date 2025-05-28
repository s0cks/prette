#ifndef PRT_TEXTURE_H
#define PRT_TEXTURE_H

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/image/image.h"
#include "prette/image/image_view.h"
#include "prette/platform.h"
#include "prette/sampler.h"
#include "prette/vk.h"

namespace prt {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
using TextureBytes = std::unique_ptr<uint8_t[], std::function<void(uint8_t*)>>;

struct TextureData {
  TextureBytes data;
  int width;
  int height;
  int num_channels;

  inline auto GetTotalBufferSize() const -> VkDeviceSize {
    return width * height * 4;
  }

  operator VkRect2D() const {
    return VkRect2D{
        .offset = {},
        .extent = operator ::VkExtent2D(),
    };
  }

  operator VkExtent2D() const {
    return {.width = width, .height = height};
  }
};

auto ReadTexture(const std::string& filename, const bool flip) -> TextureData;

static constexpr const auto kDefaultTextureName = "default-texture";
class Texture {
  friend class TextureBuilder;

 private:
  std::string name_;
  vk::Image* image_ = nullptr;
  vk::ImageView* view_ = nullptr;
  vk::Sampler* sampler_ = nullptr;

 public:
  explicit Texture(const std::string name) :
    name_(std::move(name)) {}
  Texture(const std::string name, vk::Image* image, vk::ImageView* view, vk::Sampler* sampler = nullptr);
  ~Texture();

  auto GetName() const -> const std::string& {
    return name_;
  }

  inline auto HasName() const -> bool {
    return !name_.empty();
  }

  auto GetImage() const -> vk::Image* {
    return image_;
  }

  inline auto HasImage() const -> bool {
    return vk::IsInitialized(GetImage());
  }

  auto GetImageView() const -> vk::ImageView* {
    return view_;
  }

  inline auto HasImageView() const -> bool {
    return vk::IsInitialized(GetImageView());
  }

  auto GetSampler() const -> vk::Sampler* {
    return sampler_;
  }

  inline auto HasSampler() const -> bool {
    return vk::IsInitialized(GetSampler());
  }

  void SetSampler(vk::Sampler* rhs) {
    ASSERT_INITIALIZED(rhs);
    sampler_ = rhs;
  }

  auto IsInitialized() const -> bool;
  auto ToString() const -> std::string;
};

auto NewDepthTexture(const std::string name, const VkExtent2D& extent, VkFormat format, vk::Sampler* sampler = nullptr)
    -> Texture*;

class TextureBuilder {
 private:
  std::string name_ = kDefaultTextureName;
  vk::Image* image_ = nullptr;
  vk::ImageView* view_ = nullptr;
  vk::Sampler* sampler_ = nullptr;

 public:
  explicit TextureBuilder(const std::string name = kDefaultTextureName) :
    name_(std::move(name)) {}
  ~TextureBuilder() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  inline auto HasName() const -> bool {
    return !name_.empty();
  }

  auto WithName(const std::string rhs) -> TextureBuilder& {
    ASSERT(!rhs.empty());
    name_ = std::move(rhs);
    return *this;
  }

  auto GetImage() const -> vk::Image* {
    return image_;
  }

  auto WithImage(vk::Image* rhs) -> TextureBuilder& {
    ASSERT_INITIALIZED(rhs);
    image_ = rhs;
    return *this;
  }

  inline auto HasImage() const -> bool {
    return vk::IsInitialized(GetImage());
  }

  auto GetImageView() const -> vk::ImageView* {
    return view_;
  }

  inline auto HasImageView() const -> bool {
    return vk::IsInitialized(GetImageView());
  }

  auto WithImageView(vk::ImageView* rhs) -> TextureBuilder& {
    ASSERT_INITIALIZED(rhs);
    view_ = rhs;
    return *this;
  }

  auto GetSampler() const -> vk::Sampler* {
    return sampler_;
  }

  inline auto HasSampler() const -> bool {
    return vk::IsInitialized(GetSampler());
  }

  auto WithSampler(vk::Sampler* rhs) -> TextureBuilder& {
    ASSERT_INITIALIZED(rhs);
    sampler_ = rhs;
    return *this;
  }

  auto WithTextureData(const TextureData& rhs, const VkFormat format, const bool staging = true) -> TextureBuilder&;
  auto IsValid() const -> bool;
  auto Build(const bool staging = true) -> Texture*;

  auto operator()(const bool staging = true) -> Texture* {
    return Build(staging);
  }

  operator Texture*() {
    return Build();
  }
};
}  // namespace prt

#endif  // PRT_TEXTURE_H
