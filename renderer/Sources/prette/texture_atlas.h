#ifndef PRT_TEXTURE_ATLAS_H
#define PRT_TEXTURE_ATLAS_H

#include <concepts>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/glm.h"
#include "prette/sampler.h"
#include "prette/vk.h"

namespace prt {
struct TextureWindow {
  glm::vec2 min;
  glm::vec2 max;
};

class TextureAtlas;
class AtlasTexture {
  using Resolution = glm::u32vec2;

 private:
  TextureAtlas* owner_;
  VkRect2D bounds_;

 public:
  AtlasTexture(TextureAtlas* owner, const VkRect2D bounds) :
    owner_(owner),
    bounds_(std::move(bounds)) {}
  ~AtlasTexture() = default;

  auto GetOwner() const -> TextureAtlas* {
    return owner_;
  }

  auto GetBounds() const -> const VkRect2D& {
    return bounds_;
  }

  auto GetTextureWindow() const -> TextureWindow;
};

class TextureAtlas {
 public:
  static constexpr const VkFormat kDefaultFormat = VK_FORMAT_R8G8B8A8_UNORM;

 private:
  VkExtent2D extent_;
  VkFormat format_;
  vk::Sampler* sampler_ = nullptr;
  vk::Image* image_ = nullptr;
  vk::ImageView* view_ = nullptr;
  vk::DescriptorSet* descriptors_ = nullptr;

 protected:
  virtual auto ComputeBounds(const VkExtent2D& rhs) -> VkRect2D = 0;

 public:
  TextureAtlas(const VkExtent2D extent, const VkFormat format = kDefaultFormat);
  virtual ~TextureAtlas();

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  auto GetFormat() const -> const VkFormat& {
    return format_;
  }

  auto GetImage() const -> vk::Image* {
    return image_;
  }

  auto GetImageView() const -> vk::ImageView* {
    return view_;
  }

  auto GetSampler() const -> vk::Sampler* {
    return sampler_;
  }

  auto GetDescriptorSet() const -> vk::DescriptorSet* {
    return descriptors_;
  }

  auto IsInitialized() const -> bool;
  auto Add(const fs::path path) -> AtlasTexture*;
  virtual auto Add(const VkExtent2D extent, const uint8_t* pixels) -> AtlasTexture*;

  auto GetTextureWindow() const -> TextureWindow {
    return TextureWindow{
        .min = glm::vec2(0.0f),
        .max = glm::vec2(1.0f),
    };
  }
};

class SkylineLayout {
  using Point = glm::u16vec2;

 private:
  VkExtent2D extent_{};
  std::vector<Point> skyline_{};

 public:
  explicit SkylineLayout(const VkExtent2D extent);
  ~SkylineLayout() = default;

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  auto operator()(const VkExtent2D& extent, VkRect2D* result) -> bool;
};

template <typename T>
concept TextureAtlasLayout = requires(T t, const VkExtent2D& extent, VkRect2D* result) {
  { t(extent, result) } -> std::convertible_to<bool>;
};

template <TextureAtlasLayout Layout = SkylineLayout>
class TextureAtlasTemplate : public TextureAtlas {
 private:
  Layout layout_;

 public:
  explicit TextureAtlasTemplate(Layout layout, const VkExtent2D extent, const VkFormat format = kDefaultFormat) :
    TextureAtlas(std::move(extent), format),
    layout_(std::move(layout)) {}
  ~TextureAtlasTemplate() override = default;
};
}  // namespace prt

#endif  // PRT_TEXTURE_ATLAS_H
