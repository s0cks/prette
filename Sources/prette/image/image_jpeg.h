#ifndef PRT_IMAGE_H
#error "Please #include <prette/image/image.h> instead."
#endif //PRT_IMAGE_H

#ifndef PRT_JPEG_H
#define PRT_JPEG_H

#include <string>
#include <unordered_set>

#include "prette/uri.h"
#include "prette/image/image_decoder.h"

namespace prt::img::jpeg {
  static const std::unordered_set<std::string> kValidExtensions = {
    "jpg",
    "jpeg",
  };

  static inline bool
  Filter(const uri::Uri& uri) {
    return uri.HasScheme("file")
        && uri.HasExtension(kValidExtensions);
  }

  Image* Decode(FILE* file);
  Image* Decode(const uri::Uri& uri);

  rx::observable<Image*> DecodeAsync(const uri::Uri& uri);

  static inline rx::observable<Image*>
  DecodeAsync(const uri::basic_uri& uri) {
    return DecodeAsync(uri::Uri(uri));
  }
}

#endif //PRT_JPEG_H