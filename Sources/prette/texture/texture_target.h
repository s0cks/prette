#ifndef PRT_TEXTURE_TARGET_H
#define PRT_TEXTURE_TARGET_H

#include "prette/json.h"

namespace prt::texture {
#define FOR_EACH_TEXTURE_TARGET(V) \
  V(1D, 1D)                        \
  V(2D, 2D)                        \
  V(3D, 3D)                        \
  V(CubeMap, CUBE_MAP)

  enum TextureTarget : GLenum {
#define DEFINE_TEXTURE_TARGET(Name, Target) k##Name = GL_TEXTURE_##Target,
    FOR_EACH_TEXTURE_TARGET(DEFINE_TEXTURE_TARGET)
#undef DEFINE_TEXTURE_TARGET
    kDefaultTarget = k2D,
  };

  static inline std::ostream& operator<<(std::ostream& stream, const TextureTarget& rhs) {
    switch(rhs) {
#define DEFINE_TOSTRING(Name, Target) \
      case k##Name: return stream << #Name << "(GL_TEXTURE_" << #Target << ")";
      FOR_EACH_TEXTURE_TARGET(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default: return stream << "Unknown";
    }
  }

  static inline std::optional<TextureTarget>
  ParseTextureTarget(const std::string& value) {
    if(EqualsIgnoreCase(value, "default")) return { kDefaultTarget };
#define DEFINE_PARSE_TARGET(Name, Target) \
    else if(EqualsIgnoreCase(value, #Name)) { return { k##Name }; }
    FOR_EACH_TEXTURE_TARGET(DEFINE_PARSE_TARGET)
#undef DEFINE_PARSE_TARGET
    DLOG(ERROR) << "unknown texture::TextureTarget: " << value;
    return std::nullopt;
  }

  static inline std::optional<TextureTarget>
  ParseTextureTarget(const json::Value& value) {
    if(!value.IsString())
      return std::nullopt;
    const std::string target(value.GetString(), value.GetStringLength());
    return ParseTextureTarget(target);
  }
}

#endif //PRT_TEXTURE_TARGET_H