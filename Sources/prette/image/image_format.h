#ifndef PRT_IMAGE_FORMAT_H
#define PRT_IMAGE_FORMAT_H

#include "prette/gfx.h"
#include "prette/resolution.h"

namespace prt::img {
#define FOR_EACH_IMAGE_FORMAT(V)    \
  V(RGB)                            \
  V(RGBA)

  class ImageFormat {
    typedef GLint RawType;
  private:
    RawType value_;
  public:
    constexpr ImageFormat() = default;
    constexpr ImageFormat(const RawType value):
      value_(value) {
    }
    constexpr ImageFormat(const ImageFormat& rhs) = default;
    ~ImageFormat() = default;

    constexpr RawType value() const {
      return value_;
    }

    constexpr uword GetNumberOfChannels() const {
      switch(value()) {
        case GL_RGB:
          return 3;
        case GL_RGBA:
          return 4;
        default:
          LOG(WARNING) << "Unknown TextureFormat: " << static_cast<uword>(value());
          return 0;
      }
    }

    constexpr operator RawType() const {
      return value();
    }

    ImageFormat& operator=(const ImageFormat& rhs) = default;

    constexpr bool operator==(const ImageFormat& rhs) const {
      return value() == rhs.value();
    }

    constexpr bool operator!=(const ImageFormat& rhs) const {
      return value() != rhs.value();
    }

    constexpr bool operator<(const ImageFormat& rhs) const {
      return value() < rhs.value();
    }

    constexpr bool operator>(const ImageFormat& rhs) const {
      return value() > rhs.value();
    }

    constexpr uword operator*(const Resolution& rhs) const {
      return GetNumberOfChannels() * rhs.width() * rhs.height();
    }

    friend std::ostream& operator<<(std::ostream& stream, const ImageFormat& rhs) {
      switch(rhs.value()) {
#define DEFINE_TO_STRING(Name)                    \
        case GL_##Name: return stream << #Name;
        FOR_EACH_IMAGE_FORMAT(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
        default:
          return stream << "Unknown ImageFormat: " << static_cast<uword>(rhs.value());
      }
    }
  };

#define DEFINE_FORMAT(Name)                                         \
  static constexpr const auto k##Name##Format = ImageFormat(GL_##Name);
  FOR_EACH_IMAGE_FORMAT(DEFINE_FORMAT)
#undef DEFINE_FORMAT
}

#endif //PRT_IMAGE_FORMAT_H