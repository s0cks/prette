#include "prette/texture_stitcher.h"

#include <ctime>
#include <filesystem>
#include <fmt/format.h>
#include <gflags/gflags.h>
#include <iostream>
#include <rapidjson/prettywriter.h>
#include <rapidjson/reader.h>
#include <string>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/image.h"
#include "prette/json.h"
#include "prette/stb.h"
#include "prette/to_string.h"

namespace prt {
DEFINE_bool(scan, true, "Scan the current directory for images to add to the texture atlas.");
DEFINE_bool(recursive, false, "Mark whether or not to scan recursively when scanning for images.");
DEFINE_uint32(image_padding, 1, "The padding (in px) for each image in the texture atlas.");
DEFINE_uint32(atlas_width, 4096, "The width (in px) of the output texture atlas.");
DEFINE_uint32(atlas_height, 4096, "The height (in px) of the output texture atlas.");
DEFINE_uint32(atlas_channels, 4, "The number of output channels for the texture atlas.");
DEFINE_string(atlas_encoding, "png", "The output encoding for the texture atlas.");
DEFINE_string(atlas_name, "atlas", "The filename for the texture atlas.");
DEFINE_string(out_dir, "", "The output directory for the texture atlas. Empty is current directory.");

auto StitchedImage::ToString() const -> std::string {
  ToStringHelper<StitchedImage> helper{};
  helper.AddField("name", name);
  helper.AddFieldRef("path", path);
  helper.AddField("bounds", glm::to_string(bounds));
  helper.AddField("uvs", glm::to_string(uvs));
  return helper;
}

auto StitchedAtlas::WriteTo(std::ostream& stream) const -> std::ostream& {
  rapidjson::StringBuffer sb{};
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
  writer.SetIndent(' ', 2);
  writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
  writer.SetMaxDecimalPlaces(4);
  writer.StartObject();
  {
    // atlas name
    writer.Key("name");
    writer.String(name.c_str());
    // atlas width
    writer.Key("width");
    writer.Uint(size[0]);
    // atlas height
    writer.Key("height");
    writer.Uint(size[1]);
    // atlas num_images
    writer.Key("num_images");
    writer.Uint(images.size());

    // atlas images
    writer.Key("images");
    writer.StartArray();
    for (const auto& image : images) {
      writer.StartObject();
      {
        // id
        writer.Key("id");
        writer.Uint(image.id);

        // name
        writer.Key("name");
        writer.String(image.name.c_str());

        writer.Key("bounds");
        writer.StartArray();
        for (auto idx = 0; idx < image.bounds.length(); idx++)
          writer.Uint(static_cast<int32_t>(image.bounds[idx]));
        writer.EndArray(image.bounds.length());

        // min_uv_x
        writer.Key("uvs");
        writer.StartArray();
        for (auto idx = 0; idx < image.uvs.length(); idx++)
          writer.Double(static_cast<double>(image.uvs[idx]));
        writer.EndArray(image.uvs.length());
      }
      writer.EndObject(5);
    }
    writer.EndArray(images.size());
  }
  writer.EndObject(5);
  return stream << sb.GetString() << std::endl;
}

auto StitchedAtlas::ToString() const -> std::string {
  ToStringHelper<StitchedAtlas> helper{};
  helper.AddFieldRef("name", name);
  helper.AddField("size", glm::to_string(size));
  // helper.AddFieldRef("images", images);
  return helper;
}

TextureStitcher::TextureStitcher(const std::string name, const glm::u32vec2 size) :
  name_(std::move(name)),
  size_(std::move(size)) {}

TextureStitcher::~TextureStitcher() {
  free(context_);
}

auto TextureStitcher::Stitch(const std::vector<ImageMetadata>& images) -> std::pair<StitchedAtlas, uint8_t*> {
  PackImages(images);
  std::vector<StitchedImage> stitched{};
  const auto bytes = StitchImages(images, stitched);
  return {
      StitchedAtlas{
          .name = name_,
          .size = size_,
          .images = stitched,
      },
      bytes,
  };
}

void TextureStitcher::PackImages(const std::vector<ImageMetadata>& images) {
  ASSERT_NOT_EMPTY(images);
  rects_.resize(images.size());
  nodes_.resize(images.size());
  for (auto idx = 0; idx < images.size(); idx++) {
    rects_[idx].id = idx;
    rects_[idx].w = static_cast<int>(FLAGS_image_padding) + images[idx].width;
    rects_[idx].h = static_cast<int>(FLAGS_image_padding) + images[idx].height;
  }
  context_ = (stbrp_context*)malloc(sizeof(stbrp_context));  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  stbrp_init_target(context_, static_cast<int>(FLAGS_atlas_width), static_cast<int>(FLAGS_atlas_height), &nodes_[0],
                    static_cast<int>(nodes_.size()));
  const auto all_packed = stbrp_pack_rects(context_, &rects_[0], static_cast<int>(rects_.size()));
  LOG_IF(FATAL, !all_packed) << "failed to pack all rects";
}

static inline auto GetImageName(const fs::path& path) -> std::string {
  auto filename = path.filename().string();
  const auto dotpos = filename.find_last_of('.');
  if (dotpos == std::string::npos)
    return filename;
  filename = filename.substr(0, dotpos);
  return filename;
}

auto TextureStitcher::StitchImages(const std::vector<ImageMetadata>& images, std::vector<StitchedImage>& stitched)
    -> uint8_t* {
  const auto atlas_width = static_cast<int>(GetOutputAtlasWidth());
  const auto atlas_height = static_cast<int>(GetOutputAtlasHeight());
  const auto atlas_channels = static_cast<int>(GetOutputAtlasChannels());
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const auto data = (uint8_t*)malloc(sizeof(uint8_t) * atlas_width * atlas_height * atlas_channels);
  for (auto idx = 0; idx < rects_.size(); idx++) {
    const auto& rect = rects_[idx];
    const auto& image = images[idx];
    ASSERT(rect.was_packed);
    int didx = 0;
    int bidx = 0;
    for (int y = 0; y < image.height; y++) {
      for (int x = 0; x < image.width; x++) {
        for (int c = 0; c < atlas_channels; c++) {
          didx = y * atlas_channels * image.width + x * atlas_channels + c;
          bidx = (y + rect.y) * atlas_channels * atlas_width + (x + rect.x) * atlas_channels + c;
          data[bidx] = image.data[didx];
        }
      }
    }

    glm::u32vec2 atlas_size(atlas_width, atlas_height);
    glm::u32vec2 pos(rect.x, rect.y);
    glm::u32vec2 size(rect.w, rect.h);
    stitched.push_back({
        .id = rects_[idx].id,
        .name = GetImageName(images[idx].path),
        .path = images[idx].path,
        .bounds = glm::u32vec4(pos, size),
        .uvs = glm::vec4(glm::vec2(pos) / glm::vec2(atlas_size),
                         (glm::vec2(pos) + glm::vec2(size)) / glm::vec2(atlas_size)),
    });
  }
  return data;
}
}  // namespace prt