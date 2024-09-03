#include "prette/image/image.h"

#include <cstdio>
#include <cstdlib>
#include <jerror.h>
#include <jpeglib.h>
#include <fmt/format.h>
#include "prette/image/image.h"

namespace prt::img::jpeg {
  static inline ImageFormat
  GetFormat(const int channels) {
    switch(channels) {
      case 4:
        return kRGBA;
      case 3:
      default:
        return kRGB;
    }
  }

  Image* Decode(FILE* file) {
    PRT_ASSERT(file);
    ImageFormat format;
    ImageSize size;

    unsigned long data_size;
    int channels;
    unsigned char* jdata;
    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;
    info.err = jpeg_std_error(&err);     
    jpeg_create_decompress(&info);
    jpeg_stdio_src(&info, file);    
    jpeg_read_header(&info, TRUE);
    jpeg_start_decompress(&info);
    channels = info.num_components;
    size[0] = info.output_width;
    size[1] = info.output_height;
    format = GetFormat(channels);
    const auto total_size = size[0] * size[1] * channels;
    const auto image = Image::New(format, size, total_size);
    while (info.output_scanline < info.output_height) {
      const auto row_ptr = &image->data()[channels * info.output_width * info.output_scanline];
      jpeg_read_scanlines(&info, (JSAMPARRAY)&row_ptr, 1);
    }
    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);
    return image;
  }

  Image* Decode(const uri::Uri& uri) {
    PRT_ASSERT(jpeg::Filter(uri));
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

  rx::observable<Image*> DecodeAsync(const uri::Uri& uri) {
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