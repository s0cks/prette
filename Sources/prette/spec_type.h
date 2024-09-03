#ifndef PRT_SPEC_TYPE_H
#define PRT_SPEC_TYPE_H

#include <cstdint>
#include <ostream>
#include <optional>

namespace prt::spec {
#define FOR_EACH_SPEC_TYPE(V)   \
  V(Texture2d)                  \
  V(Texture3d)                  \
  V(CubeMap)                    \
  V(Material)                   \
  V(Cursor)                     \
  V(Font)                       \
  V(VertexShader)               \
  V(FragmentShader)             \
  V(GeometryShader)             \
  V(TessControlShader)          \
  V(TessEvalShader)

  enum Type : uint8_t {
#define DEFINE_TYPE(Name) k##Name,
    FOR_EACH_SPEC_TYPE(DEFINE_TYPE)
#undef DEFINE_TYPE
  };

  constexpr const char* ToString(const Type& rhs);
  std::optional<Type> Parse(const char* value, const uint64_t length);

  static inline std::optional<Type>
  Parse(const char* value) {
    if(value == NULL)
      return std::nullopt;
    return Parse(value, strlen(value));
  }

  static inline std::optional<Type>
  Parse(const std::string& value) {
    return Parse(value.data(), value.length());
  }
  
  static inline std::ostream&
  operator<<(std::ostream& stream, const Type& rhs) {
    return stream << ToString(rhs);
  }
}

#endif //PRT_SPEC_TYPE_H