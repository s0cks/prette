#ifndef PRT_TEXTURE_FORMAT_H
#define PRT_TEXTURE_FORMAT_H

#include "prette/gfx.h"

namespace prt::texture {
#define FOR_EACH_TEXTURE_FORMAT(V)        \
  V(RGB,        GL_RGB)                   \
  V(RGBA,       GL_RGBA)

  enum TextureFormat : GLenum {
#define DEFINE_TEXTURE_FORMAT(Name, GlValue) k##Name = (GlValue),
    FOR_EACH_TEXTURE_FORMAT(DEFINE_TEXTURE_FORMAT)
#undef DEFINE_TEXTURE_FORMAT
    kDefaultTextureFormat = kRGBA,
  };

  static inline constexpr const char*
  ToString(const TextureFormat& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)               \
      case k##Name: return #Name;
      FOR_EACH_TEXTURE_FORMAT(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "Unknown TextureFormat";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const TextureFormat& rhs) {
    return stream << ToString(rhs);
  }
}

#endif //PRT_TEXTURE_FORMAT_H