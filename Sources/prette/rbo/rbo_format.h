
#ifndef PRT_RBO_FORMAT_H
#define PRT_RBO_FORMAT_H

#include "prette/gfx.h"

namespace prt::rbo {
#define FOR_EACH_RBO_COLOR_FORMAT(V)      \
  V(RGBA4,          GL_RGBA4)             \
  V(RGB5_A1,        GL_RGB5_A1)           \
  V(RGB565,         GL_RGB565)            \
  V(RGB10_A2,       GL_RGB10_A2)          \
  V(RGB10_A2UI,     GL_RGB10_A2UI)        \
  V(SRGB8_ALPHA8,   GL_SRGB8_ALPHA8)

#define FOR_EACH_RBO_DEPTH_FORMAT(V)              \
  V(DepthComponent16,   GL_DEPTH_COMPONENT16)     \
  V(DepthComponent24,   GL_DEPTH_COMPONENT24)     \
  V(DepthComponent32,   GL_DEPTH_COMPONENT32)     \
  V(DepthComponent32f,  GL_DEPTH_COMPONENT32F)

#define FOR_EACH_RBO_STENCIL_FORMAT(V)            \
  V(Depth24Stencil8,    GL_DEPTH24_STENCIL8)      \
  V(Depth32fStencil8,   GL_DEPTH32F_STENCIL8)     \
  V(StencilIndex8,      GL_STENCIL_INDEX8)

#define FOR_EACH_RBO_FORMAT(V)      \
  FOR_EACH_RBO_COLOR_FORMAT(V)      \
  FOR_EACH_RBO_DEPTH_FORMAT(V)      \
  FOR_EACH_RBO_STENCIL_FORMAT(V)

  enum RboFormat : GLenum {
#define DEFINE_FORMAT(Name, GlValue) k##Name##Format = (GlValue),
    FOR_EACH_RBO_FORMAT(DEFINE_FORMAT)
#undef DEFINE_FORMAT
  };

  static inline constexpr const char*
  ToString(const RboFormat& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue) \
      case k##Name##Format: return #Name;
      FOR_EACH_RBO_FORMAT(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "Unknown RboFormat";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const RboFormat& rhs) {
    return stream << ToString(rhs);
  }
}

#endif //PRT_RBO_FORMAT_H