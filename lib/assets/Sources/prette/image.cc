#include "prette/image.h"

#include <stb_image.h>
#include <string>
#include <utility>

#include "prette/common.h"
#include "prette/to_string.h"

namespace prt {
auto ImageMetadata::ToString() const -> std::string {
  ToStringHelper<ImageMetadata> helper{};
  helper.AddFieldRef("path", path);
  helper.AddFieldRef("width", width);
  helper.AddFieldRef("height", height);
  helper.AddFieldRef("num_channels", num_channels);
  return helper;
}

auto ReadImage(const fs::path path, const bool flip) -> ImageMetadata {
  const int force_channels = STBI_rgb_alpha;
  int width = 0, height = 0, num_channels = 0;
  ImageBytes bytes = {stbi_load(path.c_str(), &width, &height, &num_channels, force_channels),
                      (void (*)(uint8_t*))stbi_image_free};  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  return {
      .data = std::move(bytes),
      .width = width,
      .height = height,
      .num_channels = force_channels,
      .path = std::move(path),
  };
}
}  // namespace prt