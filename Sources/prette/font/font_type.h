#ifndef PRT_FONT_TYPE_H
#define PRT_FONT_TYPE_H

#include <ostream>

namespace prt::font {
#define FOR_EACH_FONT_TYPE(V) \
  V(TrueType)                 \
  V(OpenType)

  enum FontType {
#define DEFINE_FONT_TYPE(Name) k##Name,
    FOR_EACH_FONT_TYPE(DEFINE_FONT_TYPE)
#undef DEFINE_FONT_TYPE
  };

  static inline constexpr const char*
  ToString(const FontType& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name)                \
      case k##Name: return #Name;
      FOR_EACH_FONT_TYPE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "Unknown FontType";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const FontType& rhs) {
    return stream << ToString(rhs);
  }
}

#endif //PRT_FONT_TYPE_H