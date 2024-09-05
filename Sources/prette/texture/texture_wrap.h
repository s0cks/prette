#ifndef PRT_TEXTURE_WRAP_H
#define PRT_TEXTURE_WRAP_H

#include <string>

#include "prette/gfx.h"
#include "prette/json.h"
#include "prette/texture/texture_target.h"
#include "prette/texture/texture_property.h"

namespace prt::texture {
#define FOR_EACH_TEXTURE_WRAP_MODE(V)     \
  V(Repeat, GL_REPEAT)                    \
  V(ClampToEdge, GL_CLAMP_TO_EDGE)        \
  V(ClampToBorder, GL_CLAMP_TO_BORDER)

  enum TextureWrapMode : GLenum {
#define DEFINE_TEXTURE_WRAP_MODE(Name, GlValue) k##Name = (GlValue),
    FOR_EACH_TEXTURE_WRAP_MODE(DEFINE_TEXTURE_WRAP_MODE)
#undef DEFINE_TEXTURE_WRAP_MODE

    kNumberOfTextureWrapModes,
    kDefaultTextureWrapMode = kClampToEdge
  };

  static inline const char*
  GetTextureWrapModeName(const TextureWrapMode& rhs) {
    switch(rhs) {
#define DEFINE_TOSTRING(Name, GlValue) \
      case k##Name: return #Name;
      FOR_EACH_TEXTURE_WRAP_MODE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default: return "unknown";
    }
  }

  static inline std::optional<TextureWrapMode>
  GetWrapMode(const std::string& value) {
    if(EqualsIgnoreCase(value, "default")) return { kDefaultTextureWrapMode };
#define DEFINE_FROM_STRING(Name, GlValue) \
    else if(EqualsIgnoreCase(value, #Name)) return { k##Name };
    FOR_EACH_TEXTURE_WRAP_MODE(DEFINE_FROM_STRING)
#undef DEFINE_FROM_STRING
    else if(EqualsIgnoreCase(value, "clamp2edge")) return { kClampToEdge };
    else if(EqualsIgnoreCase(value, "clamp2border")) return { kClampToBorder };
    DLOG(ERROR) << "unknown TextureWrapMode: " << value;
    return std::nullopt;
  }

  static inline std::optional<TextureWrapMode>
  GetWrapMode(const json::Value& value) {
    PRT_ASSERT(value.IsString());
    const std::string val(value.GetString(), value.GetStringLength());
    return GetWrapMode(val);
  }

  template<const bool IsConst>
  static inline std::optional<TextureWrapMode>
  GetWrapMode(const json::GenericObject<IsConst, json::Document::ValueType>& obj, const char* name) {
    if(!obj.HasMember(name))
      return std::nullopt;
    const auto& property = obj[name];
    if(!property.IsString()) {
      DLOG(WARNING) << name << " is not a string.";
      return std::nullopt;
    }
    return GetWrapMode(property);
  }

  struct TextureWrap {
  public:
    TextureWrapMode r;
    TextureWrapMode s;
    TextureWrapMode t;

    constexpr TextureWrap(const TextureWrapMode R,
                          const TextureWrapMode S,
                          const TextureWrapMode T):
      r(R),
      s(S),
      t(T) {
    }
    template<const bool IsConst>
    explicit constexpr TextureWrap(const json::GenericObject<IsConst, json::Document::ValueType>& value):
      TextureWrap(GetWrapMode(value, "r").value_or(kDefaultTextureWrapMode),
                  GetWrapMode(value, "s").value_or(kDefaultTextureWrapMode),
                  GetWrapMode(value, "t").value_or(kDefaultTextureWrapMode)) {
    }
    explicit constexpr TextureWrap(const TextureWrapMode mode = kDefaultTextureWrapMode):
      TextureWrap(mode, mode, mode) {
    }
    explicit constexpr TextureWrap(const std::string& value):
      TextureWrap(GetWrapMode(value).value_or(kDefaultTextureWrapMode)) {  
    }
    TextureWrap(const TextureWrap& rhs) = default;
    ~TextureWrap() = default;

    void ApplyTo(const TextureTarget target) const {
      glTexParameteri(target, GL_TEXTURE_WRAP_R, r);
      CHECK_GL;
      glTexParameteri(target, GL_TEXTURE_WRAP_S, s);
      CHECK_GL;
      glTexParameteri(target, GL_TEXTURE_WRAP_T, t);
      CHECK_GL;
    }

    TextureWrap& operator=(const TextureWrap& rhs) = default;

    TextureWrap& operator=(const TextureWrapMode& rhs) {
      r = s = t = rhs;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& stream, const TextureWrap& rhs) {
      stream << "TextureWrap(";
      stream << "r=" << rhs.r << ", ";
      stream << "s=" << rhs.s << ", ";
      stream << "t=" << rhs.t;
      stream << ")";
      return stream;
    }

    bool operator==(const TextureWrap& rhs) const {
      return r == rhs.r
          && s == rhs.s
          && t == rhs.t;
    }

    bool operator==(const TextureWrapMode& rhs) const {
      return r == rhs
          && s == rhs
          && t == rhs;
    }

    bool operator!=(const TextureWrap& rhs) const {
      return r != rhs.r
          || s != rhs.s
          || t != rhs.t;
    }

    bool operator!=(const TextureWrapMode& rhs) const {
      return r != rhs
          || s != rhs
          || t != rhs;
    }

    template<const TextureTarget Target = k2D>
    static inline TextureWrapMode
    GetTextureWrapMode(const GLenum coord) {
      GLint mode;
      glGetTexParameteriv(Target, coord, &mode);
      CHECK_GL;
      return static_cast<TextureWrapMode>(mode);
    }

    template<const TextureTarget Target = k2D>
    static inline TextureWrap
    GetTextureWrap() {
      return {
        GetTextureWrapMode<Target>(GL_TEXTURE_WRAP_R),
        GetTextureWrapMode<Target>(GL_TEXTURE_WRAP_S),
        GetTextureWrapMode<Target>(GL_TEXTURE_WRAP_T)
      };
    }
  };

  static constexpr const auto kDefaultWrap = TextureWrap(kDefaultTextureWrapMode);
  static constexpr const auto kRepeatWrap = TextureWrap(kRepeat);
  static constexpr const auto kClampToEdgeWrap = TextureWrap(kClampToEdge);
  static constexpr const auto kClampToBorderWrap = TextureWrap(kClampToBorder);
}

#endif //PRT_TEXTURE_WRAP_H