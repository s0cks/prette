#ifndef PRT_IMAGE_FORMAT_H
#define PRT_IMAGE_FORMAT_H

#include "prette/gfx.h"

namespace prt::img {
  enum ImageFormat : GLenum {
    kRGB      =   GL_RGB,
    kRGBA     =   GL_RGBA,
    kTotalNumberOfImageFormats = 2,
    kDefaultImageFormat = kRGB,
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const ImageFormat& rhs) {
    switch(rhs) {
      case kRGB:
        return stream << "RGB";
      case kRGBA:
        return stream << "RGBA";
      default:
        return stream << "Unknown ImageFormat: " << static_cast<GLenum>(rhs);
    }
  }
}

#endif //PRT_IMAGE_FORMAT_H