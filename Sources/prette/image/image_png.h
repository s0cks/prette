#ifndef PRT_IMAGE_H
#error "Please #include <prette/texture/image/image.h> instead."
#endif //PRT_IMAGE_H

#ifndef PRT_PNG_H
#define PRT_PNG_H

#include <string>
#include <optional>
#include <unordered_set>

#include "prette/image/image.h"
#include "prette/image/image_decoder.h"

namespace prt::img::png {
  static const std::unordered_set<std::string> kValidExtensions = {
    "png",
  };

  static inline bool
  Filter(const uri::Uri& uri) {
    return uri.HasScheme("file")
        && uri.HasExtension(kValidExtensions);
  }

  Image* Decode(FILE* file);
  Image* Decode(const uri::Uri& uri);

  static inline Image*
  Decode(const uri::basic_uri& uri) {
    return Decode(uri::Uri(uri));
  }

  rx::observable<Image*> DecodeAsync(const uri::Uri& uri);

  static inline rx::observable<Image*>
  DecodeAsync(const uri::basic_uri& uri) {
    return DecodeAsync(uri::Uri(uri));
  }
}

#endif //PRT_PNG_H