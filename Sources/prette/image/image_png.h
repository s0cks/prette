#ifndef PRT_PNG_H
#define PRT_PNG_H

#include <string>
#include <optional>
#include <unordered_set>
#include "prette/image/image.h"

namespace prt::img {
  class Image;
  class Png {
    DEFINE_NON_INSTANTIABLE_TYPE(Png);
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

#endif //PRT_PNG_H