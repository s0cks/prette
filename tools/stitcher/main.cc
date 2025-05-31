#include <crc32c/crc32c.h>
#include <cstdlib>
#include <ctime>
#include <fmt/format.h>
#include <fstream>
#include <gflags/gflags.h>
#include <inja/inja.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "stitcher_flags.h"

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/image.h"
#include "prette/os_thread.h"
#include "prette/stb.h"
#include "prette/texture_stitcher.h"

using namespace prt;

static inline auto GetOutputDir() -> fs::path {
  return fs::current_path();
}

static inline auto GetOutputManifestPath() -> fs::path {
  return GetOutputDir() / fmt::format("{0:s}.json", FLAGS_atlas_name);
}

static inline auto GetOutputAtlasPath() -> fs::path {
  return GetOutputDir() / fmt::format("{0:s}.{1:s}", FLAGS_atlas_name, FLAGS_atlas_encoding);
}

static inline auto HasExtension(const std::string extension) -> fs::path_predicate {
  return [=](const fs::path& path) {
    return path.has_extension() && path.extension().compare(extension) == 0;
  };
}

static const auto kHasPngExtension = HasExtension(".png");
static const auto kHasJpegExtension = HasExtension(".jpeg");
static const auto kHasJpgExtension = HasExtension(".jpg");

static inline auto IsValidImage(const fs::path& path) -> bool {
  const auto& filename = path.filename();
  return fs::exists(path) && fs::is_regular_file(path) &&
         (kHasPngExtension(path) || kHasJpegExtension(path) || kHasJpgExtension(path)) &&
         filename != fmt::format("{0:s}.png", FLAGS_atlas_name);
}

static inline auto DiscoverImages(std::vector<ImageMetadata>& images) -> bool {
  if (FLAGS_scan || FLAGS_recursive) {
    if (FLAGS_recursive) {
      VLOG(1) << "scanning " << fs::current_path() << " recursively....";
      for (const auto& dir_entry : fs::recursive_directory_iterator(fs::current_path())) {
        if (!IsValidImage(dir_entry))
          continue;
        VLOG(1) << "processing " << dir_entry << "....";
        images.push_back(ReadImage(dir_entry));
      }
    } else {
      VLOG(1) << "scanning " << fs::current_path() << "....";
      for (const auto& dir_entry : fs::directory_iterator(fs::current_path())) {
        if (!IsValidImage(dir_entry))
          continue;
        VLOG(1) << "processing: " << dir_entry << "....";
        images.push_back(ReadImage(dir_entry));
      }
    }
  }
  return true;
}

static inline auto WriteImage(const fs::path path, const StitchedAtlas& rhs, const uint8_t* data) -> bool {
  ASSERT(data);
  stbi_write_png(path.c_str(), static_cast<int>(rhs.size[0]), static_cast<int>(rhs.size[1]), 4, &data[0], 0);
  return true;
}

static constexpr const auto kAtlasHeaderTemplate =
    "#ifndef PRT_{{ upper(atlas_name) }}_H \n"
    "#define PRT_{{ upper(atlas_name) }}_H\n"
    "\n"
    "/*************************************************************/\n"
    "/*     Do not edit. This file was auto generated using:      */\n"
    "/*     prt-texture-stitcher v{{ stitcher_version }}                           */\n"
    "/*************************************************************/\n"
    "\n"
    "#include <cstdint>\n"
    "#include \"prette/glm.h\"\n"
    "\n"
    "// {{ atlas_filename }}\n"
    "namespace prt::{{ lower(atlas_name) }} {\n"
    "static constexpr const auto kAtlasName = \"{{ atlas_name }}\";\n"
    "static constexpr const auto kAtlasChecksum = {{ atlas_checksum }};\n"
    "static constexpr const uint64_t kAtlasWidth = {{ atlas_width }};\n"
    "static constexpr const uint64_t kAtlasHeight = {{ atlas_width }};\n"
    "static constexpr const uint64_t kNumberOfChannels = {{ num_channels }};\n"
    "\n"
    "{\% for image in images %}\n"
    "// {{ image.filename }}\n"
    "struct {{ image.name }} {\n"
    "static constexpr const uint64_t kIndex = {{ image.id }};\n"
    "static constexpr const auto kName = \"{{ image.name }}\";\n"
    "static constexpr const auto kPos = glm::u32vec2({{ image.x }}, {{ image.y }});\n"
    "static constexpr const auto kSize = glm::u32vec2({{ image.width }}, {{ image.height }});\n"
    "static constexpr const auto kMinUV = glm::fvec2({{ image.min_uv.x }}f, {{ image.min_uv.y }}f);\n"
    "static constexpr const auto kMaxUV = glm::fvec2({{ image.max_uv.x }}f, {{ image.max_uv.y }}f);\n"
    "};\n"
    "{\% endfor %}\n"
    "}\n"
    "\n"
    "#endif // PRT_{{ upper(atlas_name) }}_H\n";

static inline void GenerateHeaderFile(const StitchedAtlas& atlas, uint32_t checksum, const fs::path out) {
  inja::json data{};
  data["stitcher_version"] = "0.1.0";
  data["atlas_checksum"] = fmt::format("{0:#04x}", checksum);
  data["atlas_filename"] = atlas.path.filename();
  data["atlas_name"] = atlas.name;
  data["atlas_width"] = atlas.size[0];
  data["atlas_height"] = atlas.size[1];
  data["num_channels"] = 4;

  auto all_images = inja::json::array();
  for (const auto& img : atlas.images) {
    auto i = inja::json::object();
    i["name"] = img.name;
    i["filename"] = img.path.filename();
    i["id"] = img.id;
    i["x"] = img.bounds[0];
    i["y"] = img.bounds[1];
    i["width"] = img.bounds[2];
    i["height"] = img.bounds[3];
    i["min_uv"]["x"] = img.uvs[0];
    i["min_uv"]["y"] = img.uvs[1];
    i["max_uv"]["x"] = img.uvs[2];
    i["max_uv"]["y"] = img.uvs[3];

    all_images.push_back(i);
  }
  data["images"] = all_images;

  std::fstream f(out, std::ios::out | std::ios::binary | std::ios::trunc);
  inja::render_to(f, kAtlasHeaderTemplate, data);
  f.close();
}

auto main(int argc, char** argv) -> int {
  ::google::InitGoogleLogging(argv[0]);
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  srand(time(nullptr));
  LOG_IF(FATAL, !SetCurrentThreadName("main")) << "failed to set main thread name.";

  ASSERT_NOT_EMPTY(FLAGS_atlas_name);
  ASSERT_NOT_EMPTY(FLAGS_atlas_encoding);
  const auto out_path = GetOutputAtlasPath();
  const auto atlas_name = FLAGS_atlas_name;
  glm::u32vec2 atlas_size(FLAGS_atlas_width, FLAGS_atlas_height);

  std::vector<ImageMetadata> images{};
  LOG_IF(FATAL, !DiscoverImages(images)) << "failed to discover images.";
  TextureStitcher stitcher(atlas_name, atlas_size);
  auto [atlas, bytes] = stitcher.Stitch(images);
  atlas.path = out_path;
  ASSERT(bytes);
  WriteImage(out_path, atlas, bytes);

  const auto checksum = crc32c::Crc32c(bytes, atlas.size[0] * atlas.size[1] * 4);
  if (!FLAGS_out_header_filename.empty())
    GenerateHeaderFile(atlas, checksum, fs::path(FLAGS_out_header_filename));
  return EXIT_SUCCESS;
}