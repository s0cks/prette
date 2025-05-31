#ifndef PRT_IMAGE_H
#define PRT_IMAGE_H

#include <cstdint>
#include <functional>
#include <memory>
#include <ostream>
#include <string>

#include "prette/common.h"

namespace prt {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
using ImageBytes = std::unique_ptr<uint8_t[], std::function<void(uint8_t*)>>;

struct ImageMetadata {
  ImageBytes data;
  int width;
  int height;
  int num_channels;
  fs::path path;

  inline auto GetTotalBufferSize() const -> uint64_t {
    return width * height * num_channels;
  }

  auto ToString() const -> std::string;

  friend auto operator<<(std::ostream& stream, const ImageMetadata& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};

auto ReadImage(const fs::path path, const bool flip = true) -> ImageMetadata;
}  // namespace prt

#endif  // PRT_IMAGE_H
