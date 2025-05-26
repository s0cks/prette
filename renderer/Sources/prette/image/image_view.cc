#include "prette/image/image_view.h"

#include <string>

#include "prette/gfx.h"
#include "prette/image/image_view_builder.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
ImageView::ImageView(const VkImageViewCreateInfo* create_info) {
  const auto driver = Driver::Get();
  driver->CreateImageView(create_info, handle_ptr());
}

ImageView::~ImageView() {
  const auto driver = Driver::Get();
  driver->DestroyImageView(handle_ref());
}

auto ImageView::ToString() const -> std::string {
  return ToStringHelper<ImageView>{};
}

auto NewDepthImageView(vk::Image* image, const VkFormat format) -> vk::ImageView* {
  vk::ImageViewBuilder builder(image);
  // clang-format off
  return builder.WithFormat(format)
    .WithDepthAspectMask();
  // clang-format on
}
}  // namespace prt::vk