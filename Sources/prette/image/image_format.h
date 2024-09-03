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
}

#endif //PRT_IMAGE_FORMAT_H