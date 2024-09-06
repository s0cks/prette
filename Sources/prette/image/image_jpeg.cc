#include "prette/image/image_jpeg.h"
#include <cstdio>
#include <cstdlib>
#include <jerror.h>
#include <jpeglib.h>
#include <fmt/format.h>
#include "prette/image/image.h"

namespace prt::img {
  static inline auto
  GetFormat(const int channels) -> ImageFormat {
    switch(channels) {
      case 4:
        return img::kRGBAFormat;
      case 3:
      default:
        return img::kRGBFormat;
    }
  }

  static const ExtensionSet kValidExtensions = {
    "jpeg",
    "jpg",
  };

  auto Jpeg::GetValidExtensions() -> const ExtensionSet& {
    return kValidExtensions;
  }

  auto Jpeg::Decode(FILE* file) -> Image* {
    PRT_ASSERT(file);
    ImageFormat format{};
    unsigned long data_size{};
    int channels{};
    unsigned char* jdata{};
    struct jpeg_decompress_struct info{};
    struct jpeg_error_mgr err{};
    info.err = jpeg_std_error(&err);     
    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, file);    
    jpeg_read_header(&info, TRUE);
    jpeg_start_decompress(&info);
    channels = info.num_components;
    const auto width = static_cast<int32_t>(info.output_width);
    const auto height = static_cast<int32_t>(info.output_height);
    const auto resolution = Resolution(width, height);
    format = GetFormat(channels);
    const auto image = Image::New(format, resolution);
    while (info.output_scanline < height) {
      const auto row_ptr = &image->data()[channels * width * info.output_scanline];
      // NOLINTNEXTLINE
      jpeg_read_scanlines(&info, (JSAMPARRAY) &row_ptr, 1);
    }
    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);
    return image;
  }

  auto Jpeg::Decode(const uri::Uri& uri) -> Image* {
    PRT_ASSERT(Filter(uri));
    const auto file = uri.OpenFileForReading();
    if(!file) {
      DLOG(ERROR) << "failed to open image: " << uri;
      return nullptr;
    }

    const auto image = Decode(file);
    if(!image) {
      DLOG(ERROR) << "failed to decode image: " << uri;
      return nullptr;
    }

    const auto error = fclose(file);
    LOG_IF(FATAL, error == EOF) << "failed to close file: " << uri;
    return image;
  }

  auto Jpeg::DecodeAsync(const uri::Uri& uri) -> rx::observable<Image*> {
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension(".jpeg") || uri.HasExtension(".jpg"));
    return rx::observable<>::create<Image*>([uri](rx::subscriber<Image*> s) {
      const auto file = uri.OpenFileForReading();
      if(!file) {
        const auto err = fmt::format("failed to open file: {0:s}", (const std::string&) uri);
        s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
        return;
      }

      const auto image = Decode(file);
      if(!image) {
        const auto err = fmt::format("failed to decode JPEG from file: {0:s}", (const std::string&) uri);
        s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
        return;
      }
      s.on_next(image); //TODO: flcose file and check result
      s.on_completed();
    });
  }
}