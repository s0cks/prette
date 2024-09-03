#ifndef PRT_GFX_BLEND_EQUATION_H
#define PRT_GFX_BLEND_EQUATION_H

#include <optional>
#include "prette/gfx.h"

namespace prt::gfx {
#define FOR_EACH_GFX_BLEND_EQUATION(V)              \
  V(Add,              GL_FUNC_ADD)                  \
  V(Subtract,         GL_FUNC_SUBTRACT)             \
  V(ReverseSubtract,  GL_FUNC_REVERSE_SUBTRACT)     \
  V(Min,              GL_MIN)                       \
  V(Max,              GL_MAX)

  enum BlendEquation : GLenum {
#define DEFINE_BLEND_EQUATION(Name, GlValue) k##Name = (GlValue),
    FOR_EACH_GFX_BLEND_EQUATION(DEFINE_BLEND_EQUATION)
#undef DEFINE_BLEND_EQUATION
    kTotalNumberOfBlendEquations = 5,
    kDefaultBlendEquation = kAdd,
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const BlendEquation& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)                                 \
      case k##Name: return stream << #Name << "(" << #GlValue << ")";
      FOR_EACH_GFX_BLEND_EQUATION(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "unknown gfx::BlendEquation";
    }
  }

  static inline std::optional<BlendEquation>
  ParseBlendEquation(const std::string& value) {
    if(value.empty() || EqualsIgnoreCase(value, "default"))
      return std::nullopt;
#define DEFINE_PARSE_BLEND_EQUATION(Name, GlValue)    \
    else if(EqualsIgnoreCase(value, #Name))           \
      return { k##Name };
    FOR_EACH_GFX_BLEND_EQUATION(DEFINE_PARSE_BLEND_EQUATION)
#undef DEFINE_PARSE_BLEND_EQUATION
    DLOG(WARNING) << "unknown gfx::BlendEquation: " << value;
    return std::nullopt;
  }
}

#endif //PRT_GFX_BLEND_EQUATION_H