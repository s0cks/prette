#ifndef PRT_FBO_TARGET_H
#define PRT_FBO_TARGET_H

#include "prette/gfx.h"

namespace prt::fbo {
#define FOR_EACH_FBO_TARGET(V)      \
  V(Draw, GL_DRAW_FRAMEBUFFER)      \
  V(Read, GL_READ_FRAMEBUFFER)      \
  V(Default, GL_FRAMEBUFFER)

  enum FboTarget : GLenum {
#define DEFINE_FBO_TARGET(Name, GlValue) k##Name##Target = (GlValue),
    FOR_EACH_FBO_TARGET(DEFINE_FBO_TARGET)
#undef DEFINE_FBO_TARGET
  };

  static inline constexpr const char*
  ToString(const FboTarget& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue) \
      case FboTarget::k##Name##Target: return #Name;
      FOR_EACH_FBO_TARGET(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "Unknown";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const FboTarget& rhs) {
    return stream << ToString(rhs);
  }
}

#endif //PRT_FBO_TARGET_H