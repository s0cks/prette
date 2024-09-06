#ifndef PRT_JPEG_H
#define PRT_JPEG_H

#include <string>
#include <unordered_set>
#include "prette/image/image.h"

namespace prt::img {
  class Jpeg {
    DEFINE_NON_INSTANTIABLE_TYPE(Jpeg);
  public:
    static auto GetValidExtensions() -> const ExtensionSet&;

    static inline auto
    Filter(const uri::Uri& uri) -> bool {
      return uri.HasScheme("file")
          && uri.HasExtension(GetValidExtensions());
    }

    static auto Decode(FILE* file) -> Image*;
    static auto Decode(const uri::Uri& uri) -> Image*;
    static auto DecodeAsync(const uri::Uri& uri) -> rx::observable<Image*>;
  };
}

#endif //PRT_JPEG_H