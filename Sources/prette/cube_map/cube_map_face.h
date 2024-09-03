#ifndef PRT_CUBE_MAP_FACE_H
#define PRT_CUBE_MAP_FACE_H

#include "prette/gfx.h"

namespace prt::texture {
#define FOR_EACH_CUBE_MAP_FACE(V)                         \
  V(PositiveX,        GL_TEXTURE_CUBE_MAP_POSITIVE_X)     \
  V(NegativeX,        GL_TEXTURE_CUBE_MAP_NEGATIVE_X)     \
  V(PositiveY,        GL_TEXTURE_CUBE_MAP_POSITIVE_Y)     \
  V(NegativeY,        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)     \
  V(PositiveZ,        GL_TEXTURE_CUBE_MAP_POSITIVE_Z)     \
  V(NegativeZ,        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)

  enum CubeMapFace : GLenum {
#define DEFINE_FACE(Name, GlValue) k##Name = (GlValue),
    FOR_EACH_CUBE_MAP_FACE(DEFINE_FACE)
#undef DEFINE_FACE
    kTotalNumberOfCubeMapFaces,
    // aliases
    kRight = kPositiveX,
    kLeft = kNegativeX,
    kTop = kPositiveY,
    kBottom = kNegativeY,
    kBack = kPositiveZ,
    kFront = kNegativeZ,
  };

  static inline constexpr const char*
  ToString(const CubeMapFace& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)       \
      case k##Name: return #Name;
      FOR_EACH_CUBE_MAP_FACE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "Unknown CubeMapFace.";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const CubeMapFace& rhs) {
    return stream << ToString(rhs);
  }
}

#endif //PRT_CUBE_MAP_FACE_H