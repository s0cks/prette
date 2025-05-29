#include "prette/texture.h"

#include <filesystem>
#include <stb_image.h>
#include <string>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/copy_to_image.h"
#include "prette/flags.h"
#include "prette/image/image.h"
#include "prette/image/image_builder.h"
#include "prette/image/image_layout_transition.h"
#include "prette/image/image_view.h"
#include "prette/image/image_view_builder.h"
#include "prette/platform.h"
#include "prette/sampler.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt {
auto ReadTexture(const std::string& filename, const bool flip) -> TextureData {
  const auto image_path = fs::path(FLAGS_resources) / "textures" / filename;
  const int force_channels = STBI_rgb_alpha;

  int width = 0, height = 0, num_channels = 0;
  TextureBytes bytes = {stbi_load(image_path.c_str(), &width, &height, &num_channels, force_channels),
                        (void (*)(uint8_t*))stbi_image_free};  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  return {.data = std::move(bytes), .width = width, .height = height, .num_channels = num_channels};
}

Texture::Texture(const std::string name, vk::Image* image, vk::ImageView* view, vk::Sampler* sampler) :
  name_(std::move(name)),
  image_(image),
  view_(view),
  sampler_(sampler) {
  ASSERT_NOT_EMPTY(name);
  ASSERT_INITIALIZED(image);
  ASSERT_INITIALIZED(view_);
  descriptor_.imageView = *view_;
  if (sampler_)
    descriptor_.sampler = *sampler_;
  descriptor_.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;  // TODO: this should be configurable
}

Texture::~Texture() {
  delete image_;
  delete view_;
  delete sampler_;
}

auto Texture::IsInitialized() const -> bool {
  return vk::AllInitialized(image_, view_);
}

auto Texture::ToString() const -> std::string {
  return ToStringHelper<Texture>{};
}

auto NewDepthTexture(const std::string name, const VkExtent2D& extent, VkFormat format, vk::Sampler* sampler)
    -> Texture* {
  ASSERT_NOT_EMPTY(name);
  const auto image = vk::NewDepthImage(extent, format);
  ASSERT_INITIALIZED(image);
  image->BindMemory();
  const auto view = vk::NewDepthImageView(image, format);
  return new Texture(std::move(name), image, view, sampler);
}

auto TextureBuilder::WithTextureData(const TextureData& rhs, const VkFormat format, const bool staging)
    -> TextureBuilder& {
  {
    vk::ImageBuilder builder{};
    // clang-format off
    image_ = builder.WithType(VK_IMAGE_TYPE_2D)
      .WithExtent(rhs)
      .WithFormat(format)
      .WithSampledUsage()
      .WithTransferDestUsage();
    // clang-format on
    ASSERT_INITIALIZED(image_);
  }
  {
    vk::BufferBuilder staging_builder{};
    // clang-format off
    staging_builder.WithTransferSourceUsage()
      .WithSize(rhs.GetTotalBufferSize());
    // clang-format on
    vk::ScopedBuffer staging(staging_builder);
    {
      vk::CopyBytesToBuffer copy(&rhs.data[0], rhs.GetTotalBufferSize());
      copy(staging);
    }
    {
      vk::CopyBufferToImageWithTransitions<vk::UndefinedToTransferDest, vk::TransferDestToShaderReadOnly> copy(rhs);
      copy(staging, image_);
    }
  }
  {
    vk::ImageViewBuilder builder{};
    // clang-format off
    view_ = builder.WithImage(image_)
      .WithType(VK_IMAGE_VIEW_TYPE_2D)
      .WithFormat(format);
    // clang-format on
    ASSERT_INITIALIZED(view_);
  }
  {
    vk::SamplerBuilder builder{};
    sampler_ = builder.WithLinearFilter()
                   .WithRepeatAddressMode()
                   .WithAnisotrophyDisabled()
                   .WithMaxAnisotrophy(1.0f)
                   .WithLinearMipmapMode();
    ASSERT_INITIALIZED(sampler_);
  }
  return *this;
}

auto TextureBuilder::IsValid() const -> bool {
  return HasName() && HasImage() && HasImageView() && HasSampler();
}

auto TextureBuilder::Build(const bool staging) -> Texture* {
  ASSERT(IsValid());
  return new Texture(GetName(), GetImage(), GetImageView(), GetSampler());
}
}  // namespace prt