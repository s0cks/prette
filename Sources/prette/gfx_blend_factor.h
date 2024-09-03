#ifndef PRT_GFX_BLEND_FACTOR_H
#define PRT_GFX_BLEND_FACTOR_H

#include "prette/gfx.h"

namespace prt::gfx {
#define FOR_EACH_GFX_BLEND_FACTOR(V)                                    \
  V(Zero,                     GL_ZERO)                                  \
  V(One,                      GL_ONE)                                   \
  V(SrcColor,                 GL_SRC_COLOR)                             \
  V(OneMinusSrcColor,         GL_ONE_MINUS_SRC_COLOR)                   \
  V(DstColor,                 GL_DST_COLOR)                             \
  V(OneMinusDstColor,         GL_ONE_MINUS_DST_COLOR)                   \
  V(SrcAlpha,                 GL_SRC_ALPHA)                             \
  V(OneMinusSrcAlpha,         GL_ONE_MINUS_SRC_ALPHA)                   \
  V(DstAlpha,                 GL_DST_ALPHA)                             \
  V(OneMinusDstAlpha,         GL_ONE_MINUS_DST_ALPHA)                   \
  V(ConstantColor,            GL_CONSTANT_COLOR)                        \
  V(OneMinusConstantColor,    GL_ONE_MINUS_CONSTANT_COLOR)              \
  V(ConstantAlpha,            GL_CONSTANT_ALPHA)                        \
  V(OneMinusConstantAlpha,    GL_ONE_MINUS_CONSTANT_ALPHA)              \
  V(SrcAlphaSaturate,         GL_SRC_ALPHA_SATURATE)                    \
  V(Src1Color,                GL_SRC1_COLOR)                            \
  V(OneMinusSrc1Color,        GL_ONE_MINUS_SRC1_COLOR)                  \
  V(Src1Alpha,                GL_SRC1_ALPHA)                            \
  V(OneMinusSrc1Alpha,        GL_ONE_MINUS_SRC1_ALPHA)

  enum BlendFactor : GLenum {
#define DEFINE_BLEND_FACTOR(Name, GlValue) k##Name = (GlValue),
    FOR_EACH_GFX_BLEND_FACTOR(DEFINE_BLEND_FACTOR)
#undef DEFINE_BLEND_FACTOR
    kTotalNumberOfBlendFactors = 19,
    kDefaultSFactor = kOne,
    kDefaultDFactor = kZero,
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const BlendFactor& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)                                     \
      case k##Name: return stream << #Name << " (" << #GlValue << ")";
      FOR_EACH_GFX_BLEND_FACTOR(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "unknown gfx::BlendFactor";
    }
  }

  static inline std::optional<BlendFactor>
  ParseBlendFactor(const std::string& value) {
    if(value.empty() || EqualsIgnoreCase(value, "default"))
      return std::nullopt;
#define DEFINE_PARSE_BLEND_FACTOR(Name, GlValue)                      \
    else if(EqualsIgnoreCase(value, #Name))                           \
      return { BlendFactor::k##Name };
    
    DLOG(WARNING) << "unknown gfx::BlendFactor: " << value;
    return std::nullopt;
  }
}

#endif //PRT_GFX_BLEND_FACTOR_H