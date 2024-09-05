#ifndef PRT_TEXTURE_FILTER_H
#define PRT_TEXTURE_FILTER_H

#include <string>
#include <optional>

#include "prette/gfx.h"
#include "prette/json.h"
#include "prette/texture/texture_target.h"

namespace prt::texture {
#define FOR_EACH_TEXTURE_FILTER_COMPONENT(V) \
 V(NearestMipmapNearest, GL_NEAREST_MIPMAP_NEAREST) \
 V(NearestMipmapLinear, GL_NEAREST_MIPMAP_LINEAR) \
 V(LinearMipmapNearest, GL_LINEAR_MIPMAP_NEAREST) \
 V(LinearMipmapLinear, GL_LINEAR_MIPMAP_LINEAR) \
 V(Linear, GL_LINEAR) \
 V(Nearest, GL_NEAREST)

  enum TextureFilterComponent : GLenum {
#define DEFINE_TEXTURE_FILTER_COMPONENT(Name, Value) k##Name = (Value),
    FOR_EACH_TEXTURE_FILTER_COMPONENT(DEFINE_TEXTURE_FILTER_COMPONENT)
#undef DEFINE_TEXTURE_FILTER_COMPONENT

    kDefaultMinFilter = kLinear,
    kDefaultMagFilter = kLinear,
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const TextureFilterComponent& rhs) {
    switch(rhs) {
#define DEFINE_TOSTRING(Name, Value) \
      case k##Name: return stream << #Name;
      FOR_EACH_TEXTURE_FILTER_COMPONENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default:
        return stream << "unknown TextureFilterComponent: " << static_cast<GLenum>(rhs);
    }
  }

  static inline std::optional<TextureFilterComponent>
  ParseTextureFilterComponent(const std::string& value) {
    auto name = value;
    if(name.empty()) {
      return std::nullopt;
    }
    else if(EqualsIgnoreCase(name, "Default")) { return { kLinear }; }
#define DEFINE_PARSE_TEXTURE_FILTER(Name, Value) \
    else if(EqualsIgnoreCase(name, #Name)) { return { k##Name }; }
    FOR_EACH_TEXTURE_FILTER_COMPONENT(DEFINE_PARSE_TEXTURE_FILTER)
#undef DEFINE_PARSE_TEXTURE_FILTER
    DLOG(ERROR) << "unknown TextureFilterComponent: " << name;
    return std::nullopt;
  }

  struct TextureFilter {
  private:
    static constexpr const auto kMinFilterPropertyName = "min";
    static constexpr const auto kMagFilterPropertyName = "mag";
    static inline std::optional<TextureFilterComponent>
    GetFilterComponent(const json::Value& value) {
      PRT_ASSERT(value.IsString());
      const std::string val(value.GetString(), value.GetStringLength());
      return ParseTextureFilterComponent(val);
    }

    template<const bool IsConst>
    static inline std::optional<TextureFilterComponent>
    GetFilterComponent(const json::GenericObject<IsConst, json::Document::ValueType>& obj, const char* name) {
      if(!obj.HasMember(name))
        return std::nullopt;
      const auto& property = obj[name];
      if(!property.IsString()) {
        DLOG(WARNING) << name << " is not a string.";
        return std::nullopt;
      }
      return GetFilterComponent(property);
    }
  public:
    TextureFilterComponent min;
    TextureFilterComponent mag;

    constexpr TextureFilter(const TextureFilterComponent min_filter,
                            const TextureFilterComponent mag_filter):
      min(min_filter),
      mag(mag_filter) {
    }
    constexpr TextureFilter():
      TextureFilter(kDefaultMinFilter, kDefaultMagFilter) {
    }
    explicit constexpr TextureFilter(const TextureFilterComponent component):
      TextureFilter(component, component) {
    }
    explicit constexpr TextureFilter(const std::string& value):
      TextureFilter(ParseTextureFilterComponent(value).value_or(kDefaultMinFilter), 
                    ParseTextureFilterComponent(value).value_or(kDefaultMagFilter)) { //TODO: remove double parsing?
    }
    template<const bool IsConst>
    explicit constexpr TextureFilter(const json::GenericObject<IsConst, json::Document::ValueType>& value):
      TextureFilter(GetFilterComponent(value, kMinFilterPropertyName).value_or(kDefaultMinFilter),
                    GetFilterComponent(value, kMagFilterPropertyName).value_or(kDefaultMagFilter)) {
    }
    TextureFilter(const TextureFilter& rhs) = default;
    ~TextureFilter() = default;

    void ApplyTo(const TextureTarget target) const {
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min);
      CHECK_GL;
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag);
      CHECK_GL;
    }

    friend std::ostream& operator<<(std::ostream& stream, const TextureFilter& rhs) {
      stream << "TextureFilter(";
      stream << "min=" << rhs.min << ", ";
      stream << "mag=" << rhs.mag;
      stream << ")";
      return stream;
    }

    TextureFilter& operator=(const TextureFilter& rhs) = default;

    TextureFilter& operator=(const TextureFilterComponent v) {
      min = mag = v;
      return *this;
    }

    bool operator==(const TextureFilter& rhs) const {
      return min == rhs.min
          && mag == rhs.mag;
    }

    bool operator==(const TextureFilterComponent& rhs) const {
      return min == rhs
          && mag == rhs;
    }

    bool operator!=(const TextureFilter& rhs) const {
      return min != rhs.min
          || mag != rhs.mag;
    }

    bool operator!=(const TextureFilterComponent& rhs) const {
      return min != rhs
          || mag != rhs;
    }
  };

  static constexpr const auto kDefaultFilter = TextureFilter();
  static constexpr const auto kLinearFilter = TextureFilter(kLinear);
  static constexpr const auto kNearestFilter = TextureFilter(kNearest);
}

#endif //PRT_TEXTURE_FILTER_H