#ifndef PRT_TEXTURE_STITCHER_H
#define PRT_TEXTURE_STITCHER_H

#include <gflags/gflags.h>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/glm.h"
#include "prette/image.h"
#include "prette/stb.h"

namespace prt {
DECLARE_bool(scan);
DECLARE_bool(recursive);
DECLARE_uint32(image_padding);
DECLARE_uint32(atlas_width);
DECLARE_uint32(atlas_height);
DECLARE_uint32(atlas_channels);
DECLARE_string(atlas_encoding);
DECLARE_string(atlas_name);
DECLARE_string(out_dir);

static inline auto GetOutputAtlasWidth() -> uint32_t {
  return FLAGS_atlas_width;
}

static inline auto GetOutputAtlasHeight() -> uint32_t {
  return FLAGS_atlas_height;
}

static inline auto GetOutputAtlasChannels() -> uint32_t {
  return FLAGS_atlas_channels;
}

static inline auto GetOutputAtlasSizeInBytes() -> uint32_t {
  return GetOutputAtlasWidth() * GetOutputAtlasHeight() * GetOutputAtlasChannels();
}

struct StitchedImage {
  int id;
  std::string name;
  fs::path path;
  glm::u32vec4 bounds;
  glm::vec4 uvs;

  auto ToString() const -> std::string;
  friend auto operator<<(std::ostream& stream, const StitchedImage& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};

struct StitchedAtlas {
  std::string name;
  fs::path path;
  glm::u32vec2 size;
  std::vector<StitchedImage> images;

  auto WriteTo(std::ostream& stream) const -> std::ostream&;
  auto ToString() const -> std::string;
};

class TextureStitcher {
 private:
  std::string name_;
  glm::u32vec2 size_;
  stbrp_context* context_ = nullptr;
  std::vector<stbrp_rect> rects_{};
  std::vector<stbrp_node> nodes_{};

  void PackImages(const std::vector<ImageMetadata>& images);
  auto StitchImages(const std::vector<ImageMetadata>& images, std::vector<StitchedImage>& stiched) -> uint8_t*;

 public:
  TextureStitcher(const std::string name, const glm::u32vec2 size);
  ~TextureStitcher();

  auto Stitch(const std::vector<ImageMetadata>& images) -> std::pair<StitchedAtlas, uint8_t*>;
};
}  // namespace prt

#endif  // PRT_TEXTURE_STITCHER_H
