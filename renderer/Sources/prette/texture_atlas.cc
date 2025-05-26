#include "prette/texture_atlas.h"

#include <stb_image.h>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/copy_to_image.h"
#include "prette/descriptor_set.h"
#include "prette/glm.h"
#include "prette/image/image.h"
#include "prette/image/image_builder.h"
#include "prette/image/image_layout_transition.h"
#include "prette/image/image_view.h"
#include "prette/image/image_view_builder.h"
#include "prette/sampler.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt {
auto AtlasTexture::GetTextureWindow() const -> TextureWindow {
  const auto& bounds = GetBounds();
  const auto& owner_extent = GetOwner()->GetExtent();
  float x0 = static_cast<float>(bounds.offset.x) / static_cast<float>(owner_extent.width);
  float y0 = static_cast<float>(bounds.offset.y) / static_cast<float>(owner_extent.height);
  float x1 = x0 + static_cast<float>(bounds.extent.width) / static_cast<float>(owner_extent.width);
  float y1 = y0 + static_cast<float>(bounds.extent.height) / static_cast<float>(owner_extent.height);
  return TextureWindow{
      .min = glm::vec2(x0, y0),
      .max = glm::vec2(x1, y1),
  };
}

TextureAtlas::TextureAtlas(const VkExtent2D extent, const VkFormat format) :
  extent_(std::move(extent)),
  format_(format) {
  {
    vk::ImageBuilder builder{};
    // clang-format off
    image_ = builder.WithExtent(extent_)
                 .WithFormat(format_)
                 .WithOptimalTiling()
                 .WithUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                 .Build();
    image_->BindMemory();
    // clang-format on
    ASSERT_INITIALIZED(image_);
    vk::ApplyImageTransition<vk::UndefinedToTransferDest>(image_);
    vk::ApplyImageTransition<vk::TransferDestToShaderReadOnly>(image_);
  }
  {
    vk::ImageViewBuilder builder{};
    // clang-format off
    view_ = builder.WithImage(image_)
      .WithFormat(format_);
    // clang-format on
    ASSERT_INITIALIZED(view_);
  }
  {
    vk::SamplerBuilder builder{};
    sampler_ = builder;
    ASSERT_INITIALIZED(sampler_);
  }
  {
    vk::DescriptorSetBuilder builder{};
    const auto sampler_binding = builder.AddCombinedImageSamplerBinding(VK_SHADER_STAGE_FRAGMENT_BIT);
    descriptors_ = builder;
    ASSERT_INITIALIZED(descriptors_);
  }
}

TextureAtlas::~TextureAtlas() {
  delete sampler_;
  delete image_;
  delete view_;
  delete descriptors_;
}

auto TextureAtlas::IsInitialized() const -> bool {
  return vk::IsInitialized(sampler_) && vk::IsInitialized(view_) && vk::IsInitialized(image_) &&
         vk::IsInitialized(descriptors_);
}

auto TextureAtlas::Add(const VkExtent2D extent, const uint8_t* pixels) -> AtlasTexture* {
  const auto total_size = vk::CalcImageArea(extent);
  const auto bounds = ComputeBounds(extent);
  vk::Buffer* staging = nullptr;
  {
    vk::BufferBuilder builder{};
    staging = builder.BuildTransferDestBuffer(total_size);
    ASSERT_INITIALIZED(staging);
  }

  {
    vk::ReadOnlyShaderImageTransferDestScope layout_scope(image_);
    vk::CopyBufferToImage copy(bounds);
    copy(staging, image_);
  }
  delete staging;
  return new AtlasTexture(this, bounds);
}

auto TextureAtlas::Add(const fs::path path) -> AtlasTexture* {
  int width = 0, height = 0, num_channels = 0;
  const auto pixels = stbi_load(path.c_str(), &width, &height, &num_channels, STBI_rgb_alpha);
  if (!pixels)
    throw std::runtime_error("failed to load texture image");
  auto result = Add(
      VkExtent2D{
          .width = width,
          .height = height,
      },
      pixels);
  stbi_image_free(pixels);
  return result;
}

SkylineLayout::SkylineLayout(const VkExtent2D extent) :
  extent_(std::move(extent)) {
  skyline_.reserve(extent_.width);
  skyline_.emplace_back(0);
}

auto SkylineLayout::operator()(const VkExtent2D& extent, VkRect2D* result) -> bool {
  return false;
}
}  // namespace prt