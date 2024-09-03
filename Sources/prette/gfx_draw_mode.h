#ifndef PRT_GFX_DRAW_MODE_H
#define PRT_GFX_DRAW_MODE_H

#include "prette/gfx.h"

namespace prt::gfx {
#define FOR_EACH_DRAW_MODE(V)                                   \
  V(Points,                     GL_POINTS)                      \
  V(LineStrip,                  GL_LINE_STRIP)                  \
  V(LineLoop,                   GL_LINE_LOOP)                   \
  V(Lines,                      GL_LINES)                       \
  V(LineStripAdjacency,         GL_LINE_STRIP_ADJACENCY)        \
  V(LinesAdjacency,             GL_LINES_ADJACENCY)             \
  V(TriangleStrip,              GL_TRIANGLE_STRIP)              \
  V(TriangleFan,                GL_TRIANGLE_FAN)                \
  V(Triangles,                  GL_TRIANGLES)                   \
  V(TriangleStripAdjacency,     GL_TRIANGLE_STRIP_ADJACENCY)    \
  V(TrianglesAdjacency,         GL_TRIANGLES_ADJACENCY)         \
  V(Patches,                    GL_PATCHES)

  enum DrawMode : GLenum {
#define DEFINE_DRAW_MODE(Name, GlValue) k##Name##Mode = (GlValue),
    FOR_EACH_DRAW_MODE(DEFINE_DRAW_MODE)
#undef DEFINE_DRAW_MODE
    kNumberOfDrawModes = 12,
  };

  static inline const char*
  ToString(const DrawMode& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)     \
      case k##Name##Mode: return #Name;
      FOR_EACH_DRAW_MODE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "Unknown gfx::DrawMode";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const DrawMode& rhs) {
    return stream << ToString(rhs);
  }
}

#endif //PRT_GFX_DRAW_MODE_H