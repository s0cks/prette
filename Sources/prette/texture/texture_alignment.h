#ifndef PRT_TEXTURE_ALIGNMENT_H
#define PRT_TEXTURE_ALIGNMENT_H

#include <string>
#include <optional>

#include "prette/gfx.h"
#include "prette/json.h"

namespace prt::texture {
  enum TextureAlignment : GLint {
    kNone = 0,
    k1 = 1,
    k2 = 2,
    k4 = 4,
    k8 = 8,

    kByteAlignment = k1,
    kRowAlignment = k2,
    kWordAlignment = k4,
    kDoubleWordAlignment = k8,

    kDefaultPackAlignment = k4,
    kDefaultUnpackAlignment = k4,
  };

  static inline std::optional<TextureAlignment>
  ParseTextureAlignment(const std::string& value) {
    if(value.empty() || value.length() <= 0)
      return std::nullopt;
    if(EqualsIgnoreCase(value, "1"))
      return { k1 };
    else if(EqualsIgnoreCase(value, "2"))
      return { k2 };
    else if(EqualsIgnoreCase(value, "4"))
      return { k4 };
    else if(EqualsIgnoreCase(value, "8"))
      return { k8 };
    else if(EqualsIgnoreCase(value, "byte"))
      return { kByteAlignment };
    else if(EqualsIgnoreCase(value, "row"))
      return { kRowAlignment };
    else if(EqualsIgnoreCase(value, "word"))
      return { kWordAlignment };
    else if(EqualsIgnoreCase(value, "doubleword") || EqualsIgnoreCase(value, "dword"))
      return { kDoubleWordAlignment };
    else if(EqualsIgnoreCase(value, "default"))
      return { k4 }; //TODO fix
    return std::nullopt;
  }

  static inline std::ostream& operator<<(std::ostream& stream, const TextureAlignment& rhs) {
    switch(rhs) {
      case kNone:
        return stream << "None";
      case k1:
        return stream << "1";
      case k2:
        return stream << "2";
      case k4:
        return stream << "4";
      case k8:
        return stream << "8";
      default:
        return stream << "unknown TextureAlignment: " << static_cast<GLint>(rhs);
    }
  }

  struct PixelStoreAlignment {
    TextureAlignment pack;
    TextureAlignment unpack;

    constexpr PixelStoreAlignment(const TextureAlignment pack_alignment,
                                  const TextureAlignment unpack_alignment):
                        pack(pack_alignment),
                        unpack(unpack_alignment) {
    }
    constexpr PixelStoreAlignment():
      PixelStoreAlignment(kDefaultPackAlignment, kDefaultUnpackAlignment) {
    }
    PixelStoreAlignment(const PixelStoreAlignment& rhs) = default;
    ~PixelStoreAlignment() = default;

    void Apply() const {
      glPixelStorei(GL_PACK_ALIGNMENT, pack);
      CHECK_GL(FATAL);
      glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
      CHECK_GL(FATAL);
    }

    PixelStoreAlignment& operator=(const PixelStoreAlignment& rhs) = default;

    PixelStoreAlignment& operator=(const TextureAlignment& rhs) {
      pack = unpack = rhs;
      return *this;
    }

    bool operator==(const PixelStoreAlignment& rhs) const {
      return pack == rhs.pack
          && unpack == rhs.unpack;
    }

    bool operator==(const TextureAlignment& rhs) const {
      return pack == rhs
          && unpack == rhs;
    }

    bool operator!=(const PixelStoreAlignment& rhs) const {
      return pack != rhs.pack
          || unpack != rhs.unpack;
    }

    bool operator!=(const TextureAlignment& rhs) const {
      return pack != rhs
          || unpack != rhs;
    }

    friend std::ostream& operator<<(std::ostream& stream, const PixelStoreAlignment& rhs) {
      stream << "PixelStoreAlignment(";
      stream << "pack=" << rhs.pack << ", ";
      stream << "unpack=" << rhs.unpack;
      stream << ")";
      return stream;
    }
  };

  static constexpr const auto kDefaultAlignment = PixelStoreAlignment();

  class JsonPixelStoreAlignment {
    DEFINE_NON_COPYABLE_TYPE(JsonPixelStoreAlignment);
  protected:
    const json::Value& value_;

    inline std::optional<TextureAlignment>
    GetTextureAlignment(const char* name) const {
      PRT_ASSERT(value_.IsObject());
      if(!value_.HasMember(name))
        return std::nullopt;
      const auto& value = value_[name];
      if(value.IsInt()) {
        const auto v = value.GetInt();
        return { static_cast<TextureAlignment>(v) };
      } else if(value.IsString()) {
        const auto v = std::string(value.GetString(), value.GetStringLength());
        return ParseTextureAlignment(v);
      }
      return std::nullopt;
    }
  public:
    JsonPixelStoreAlignment() = delete;
    explicit JsonPixelStoreAlignment(const json::Value& value):
      value_(value) {
    }
    explicit JsonPixelStoreAlignment(const json::Document& doc):
      JsonPixelStoreAlignment((const json::Value&) doc) {
    }
    ~JsonPixelStoreAlignment() = default;

    explicit operator PixelStoreAlignment() const {
      PixelStoreAlignment alignment;
      if(value_.IsString()) {
        const auto v = std::string(value_.GetString(), value_.GetStringLength());
        const auto& value = ParseTextureAlignment(v).value_or(k4);
        alignment.pack = value;
        alignment.unpack = value;
      } else if(value_.IsNumber()) {
        const auto value = static_cast<TextureAlignment>(value_.GetInt());
        alignment.pack = value;
        alignment.unpack = value;
      } else if(value_.IsObject()) {
        alignment.pack = GetTextureAlignment("pack").value_or(kDefaultPackAlignment);
        alignment.unpack = GetTextureAlignment("unpack").value_or(kDefaultUnpackAlignment);
      }
      return alignment;
    }
  };
}

#endif //PRT_TEXTURE_ALIGNMENT_H