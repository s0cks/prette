#ifndef PRT_GFX_USAGE_H
#define PRT_GFX_USAGE_H

#include "prette/gfx.h"

namespace prt::gfx {
#define FOR_EACH_GFX_USAGE(V)             \
  V(StreamDraw,     GL_STREAM_DRAW)       \
  V(StreamRead,     GL_STREAM_READ)       \
  V(StreamCopy,     GL_STREAM_COPY)       \
  V(StaticDraw,     GL_STATIC_DRAW)       \
  V(StaticRead,     GL_STATIC_READ)       \
  V(StaticCopy,     GL_STATIC_COPY)       \
  V(DynamicDraw,    GL_DYNAMIC_DRAW)      \
  V(DynamicRead,    GL_DYNAMIC_READ)      \
  V(DynamicCopy,    GL_DYNAMIC_COPY)

  enum Usage : GLenum {
#define DEFINE_USAGE(Name, GlValue) k##Name##Usage = (GlValue),
    FOR_EACH_GFX_USAGE(DEFINE_USAGE)
#undef DEFINE_USAGE
    kTotalNumberOfUsages,
    kDefaultUsage = kDynamicDrawUsage,
  };

  static inline const char*
  ToString(const Usage rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)           \
      case Usage::k##Name##Usage: return #Name;
      FOR_EACH_GFX_USAGE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "unknown gfx::Usage";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const Usage& rhs) {
    return stream << ToString(rhs);
  }

  static inline bool
  IsStreamUsage(const Usage& rhs) {
    switch(rhs) {
      case kStreamCopyUsage:
      case kStreamReadUsage:
      case kStreamDrawUsage:
        return true;
      default:
        return false;
    }
  }

  static inline bool
  IsStaticUsage(const Usage& rhs) {
    switch(rhs) {
      case kStaticCopyUsage:
      case kStaticReadUsage:
      case kStaticDrawUsage:
        return true;
      default:
        return false;
    }
  }

  static inline bool
  IsDynamicUsage(const Usage& rhs) {
    switch(rhs) {
      case kDynamicReadUsage:
      case kDynamicCopyUsage:
      case kDynamicDrawUsage:
        return true;
      default:
        return false;
    }
  }
}

#endif //PRT_GFX_USAGE_H