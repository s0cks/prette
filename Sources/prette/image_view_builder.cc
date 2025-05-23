#include "prette/image_view_builder.h"

#include <vector>

#include "prette/common.h"
#include "prette/image.h"
#include "prette/image_view.h"
#include "prette/vk.h"

namespace prt::vk {
ImageViewBuilder::ImageViewBuilder() :
  vk::HandleBuilderTemplate<VkImageViewCreateInfo, ImageView>() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info_ptr()->image = VK_NULL_HANDLE;
  info_ptr()->viewType = VK_IMAGE_VIEW_TYPE_2D;
  info_ptr()->format = VK_FORMAT_UNDEFINED;
  info_ptr()->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info_ptr()->subresourceRange.baseMipLevel = 0;
  info_ptr()->subresourceRange.levelCount = 1;
  info_ptr()->subresourceRange.baseArrayLayer = 0;
  info_ptr()->subresourceRange.layerCount = 1;
}

ImageViewBuilder::ImageViewBuilder(Image* image) :
  ImageViewBuilder() {
  ASSERT_INITIALIZED(image);
  WithImage(image);
}

auto ImageViewBuilder::WithImage(const vk::Image* rhs) -> ImageViewBuilder& {
  ASSERT(rhs);
  return WithImage((*rhs));
}

auto ImageViewBuilder::IsValid() const -> bool {
  return info().format != VK_FORMAT_UNDEFINED && info().image != VK_NULL_HANDLE;
}

auto ImageViewBuilder::Build() -> ImageView* {
  ASSERT(IsValid());
  return new ImageView(&info());
}

void ImageViewBuilder::BuildAll(const std::vector<VkImage>& images, std::vector<ImageView*>& results) {
  ASSERT(!images.empty());
  results.resize(images.size());
  for (auto idx = 0; idx < images.size(); idx++) {
    // clang-format off
    results[idx] = WithImage(images[idx])
      .Build();
    // clang-format on
    ASSERT_INITIALIZED(results[idx]);
  }
}

void ImageViewBuilder::BuildAll(const std::vector<Image*>& images, std::vector<ImageView*>& results) {
  ASSERT(!images.empty());
  results.resize(images.size());
  for (auto idx = 0; idx < images.size(); idx++) {
    // clang-format off
    results[idx] = WithImage(images[idx])
      .Build();
    // clang-format on
    ASSERT_INITIALIZED(results[idx]);
  }
}
}  // namespace prt::vk