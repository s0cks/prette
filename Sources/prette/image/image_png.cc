#include "prette/image/image.h"

#include <png.h>
#include <string>
#include <fmt/format.h>
#include <glog/logging.h>
#include "prette/image/image.h"

namespace prt::img::png {
  Image* Decode(FILE* file) {
    if(!file) {
      DLOG(ERROR) << "failed to load png from null file.";
      return nullptr;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) {
      DLOG(ERROR) << "failed to load png texture from file.";
      return nullptr;
    }

    png_infop info = png_create_info_struct(png);
    if(!info) {
      DLOG(ERROR) << "failed to load png texture from file.";
      png_destroy_read_struct(&png, NULL, NULL);
      return nullptr;
    }

    if(setjmp(png_jmpbuf(png))) {
      DLOG(ERROR) << "failed to load png texture from file.";
      png_destroy_read_struct(&png, &info, NULL);
      return nullptr;
    }

    png_init_io(png, file);
    png_set_sig_bytes(png, 0);
    png_read_png(png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    ImageFormat format;

    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
    switch(color_type) {
      case PNG_COLOR_TYPE_RGBA:
        format = kRGBAFormat;
        break;
      case PNG_COLOR_TYPE_RGB:
      case PNG_COLOR_TYPE_GRAY:
        format = kRGBFormat;
        break;
      default:
        DLOG(ERROR) << "unknown color type: " << color_type;
        return nullptr;
    }

    // if(bit_depth == 16)
    //   png_set_strip_16(png);
    // if(color_type == PNG_COLOR_TYPE_PALETTE)
    //   png_set_palette_to_rgb(png);
    // if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    //   png_set_expand_gray_1_2_4_to_8(png);
    // if(png_get_valid(png, info, PNG_INFO_tRNS))
    //   png_set_tRNS_to_alpha(png);
    // if(color_type == PNG_COLOR_TYPE_RGB ||
    //    color_type == PNG_COLOR_TYPE_GRAY ||
    //    color_type == PNG_COLOR_TYPE_PALETTE) {
    //   png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    // }
    // if(color_type == PNG_COLOR_TYPE_GRAY ||
    //    color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    //   png_set_gray_to_rgb(png);
    // }

    const auto row_bytes = png_get_rowbytes(png, info);
    const auto total_size = row_bytes * height;
    const auto resolution = Resolution(width, height);
    const auto image = Image::New(format, resolution);
    png_bytepp rows = png_get_rows(png, info);
    for(auto i = 0; i < height; i++) {
      memcpy(image->data() + (row_bytes * (height - 1 - i)), rows[i], row_bytes);
    }
    png_destroy_read_struct(&png, &info, NULL);
    return image;
  }

  Image* Decode(const uri::Uri& uri) {
    PRT_ASSERT(Filter(uri));
    const auto file = uri.OpenFileForReading();
    if(!file) {
      DLOG(ERROR) << "failed to open file: " << uri;
      return nullptr;
    }

    const auto image = Decode(file);
    if(!image) {
      DLOG(ERROR) << "failed to decode png from file: " << uri;
      return nullptr;
    }

    const auto error = fclose(file);
    LOG_IF(FATAL, error == EOF) << "failed to close file: " << uri;
    return image;
  }

  rx::observable<Image*> DecodeAsync(const uri::Uri& uri) {
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension(".png"));
    return rx::observable<>::create<Image*>([uri](rx::subscriber<Image*> s) {
      const auto file = uri.OpenFileForReading();
      if(!file) {
        const auto err = fmt::format("failed to open file: {0:s}", (const std::string&) uri);
        s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
        return;
      }

      const auto image = Decode(file);
      if(!image) {
        const auto err = fmt::format("failed to decode PNG from file: {0:s}", (const std::string&) uri);
        s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
        return;
      }

      const auto error = fclose(file);
      if(error == EOF) {
        const auto err = fmt::format("failed to close image file: {0:s}", (const std::string&) uri);
        s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
        return;
      }

      s.on_next(image);
      s.on_completed();
    });
  }
}