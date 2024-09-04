#ifndef PRT_TEXTURE_FORMAT_H
#define PRT_TEXTURE_FORMAT_H

#include "prette/gfx.h"

namespace prt::texture {
  class TextureFormat {
  private:
    GLenum value_;
  public:
    constexpr TextureFormat() = default;
    constexpr TextureFormat(const GLenum value):
      value_(value) {
    }
    constexpr TextureFormat(const TextureFormat& rhs):
      value_(rhs.value()) {
    }
    ~TextureFormat() = default;

    constexpr GLenum value() const {
      return value_;
    }

    constexpr bool IsDepthFormat() const;
    constexpr bool IsStencilFormat() const;
    constexpr bool IsSRGB() const;
    constexpr bool IsUnsignedNormalized() const;
    constexpr bool IsSignedInt() const;
    constexpr bool IsUnsignedInt() const;
    constexpr bool IsFloatingPoint() const;

    TextureFormat& operator=(const TextureFormat& rhs) = default;

    constexpr bool operator==(const TextureFormat& rhs) const {
      return value() == rhs.value();
    }

    constexpr bool operator==(const GLenum& rhs) const {
      return value() == rhs;
    }

    constexpr bool operator!=(const TextureFormat& rhs) const {
      return value() != rhs.value();
    }

    constexpr bool operator!=(const GLenum& rhs) const {
      return value() != rhs;
    }

    constexpr bool operator<(const TextureFormat& rhs) const {
      return value() < rhs.value();
    }

    constexpr bool operator<(const GLenum& rhs) const {
      return value() < rhs;
    }

    constexpr bool operator>(const TextureFormat& rhs) const {
      return value() > rhs.value();
    }

    constexpr bool operator>(const GLenum& rhs) const {
      return value() > rhs;
    }

    constexpr explicit operator GLenum() const {
      return value_;
    }

    friend std::ostream& operator<<(std::ostream& stream, const TextureFormat& rhs) {
      switch(rhs.value()) {
        case GL_RG:
          return stream << "RG";
        case GL_RGB:
          return stream << "RGB";
        case GL_RGBA:
          return stream << "RGBA";
        default:
          return stream << "unknwon texture::TextureFormat: " << rhs.value();
      }
    }
  };

  static constexpr const auto kRG = TextureFormat(GL_RG);
  static constexpr const auto kRGB = TextureFormat(GL_RGB);
  static constexpr const auto kRGBA = TextureFormat(GL_RGBA);

  static constexpr const auto kDefaultTextureFormat = kRGBA;
}

#endif //PRT_TEXTURE_FORMAT_H