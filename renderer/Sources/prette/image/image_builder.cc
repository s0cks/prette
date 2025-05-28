#include "prette/image/image_builder.h"

#include "prette/assertions.h"
#include "prette/image/image.h"
#include "prette/vk.h"

namespace prt::vk {
ImageBuilder::ImageBuilder() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info_ptr()->imageType = VK_IMAGE_TYPE_2D;
  info_ptr()->extent = {};
  info_ptr()->mipLevels = 1;
  info_ptr()->arrayLayers = 1;
  info_ptr()->format = VK_FORMAT_UNDEFINED;
  info_ptr()->tiling = VK_IMAGE_TILING_OPTIMAL;
  info_ptr()->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info_ptr()->usage = 0;
  info_ptr()->samples = VK_SAMPLE_COUNT_1_BIT;
  info_ptr()->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

auto ImageBuilder::IsValid() const -> bool {
  return info().format != VK_FORMAT_UNDEFINED && CalcArea(info().extent) != 0;
}

auto ImageBuilder::Build() -> Image* {
  ASSERT(IsValid());
  return new Image(&info(), mem_flags_);
}
}  // namespace prt::vk