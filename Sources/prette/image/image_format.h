#ifndef PRT_IMAGE_FORMAT_H
#define PRT_IMAGE_FORMAT_H

#include "prette/gfx.h"
#include "prette/resolution.h"

namespace prt::img {
#define FOR_EACH_IMAGE_FORMAT(V)    \
  V(RGB)                            \
  V(RGBA)

  class ImageFormat {
  public:
    enum Type {
#define DEFINE_TYPE(Name) k##Name,
      FOR_EACH_IMAGE_FORMAT(DEFINE_TYPE)
#undef DEFINE_TYPE
    };
  private:
    Type value_;
  public:
    constexpr ImageFormat() = default;
    constexpr ImageFormat(const Type value):
      value_(value) {
    }
    constexpr ImageFormat(ImageFormat&& rhs) = default;
    constexpr ImageFormat(const ImageFormat& rhs) = default;
    ~ImageFormat() = default;

    constexpr auto value() const -> Type {
      return value_;
    }

    constexpr auto GetNumberOfChannels() const -> uword {
      switch(value()) {
        case kRGB:
          return 3;
        case kRGBA:
          return 4;
        default:
          LOG(WARNING) << "Unknown TextureFormat: " << static_cast<uword>(value());
          return 0;
      }
    }

    constexpr operator Type () const {
      return value();
    }

    auto operator=(const ImageFormat& rhs) -> ImageFormat& = default;
    auto operator=(ImageFormat&& rhs) -> ImageFormat& = default;

    constexpr auto operator==(const ImageFormat& rhs) const -> bool {
      return value() == rhs.value();
    }

    constexpr auto operator!=(const ImageFormat& rhs) const -> bool {
      return value() != rhs.value();
    }

    constexpr auto operator<(const ImageFormat& rhs) const -> bool {
      return value() < rhs.value();
    }

    constexpr auto operator>(const ImageFormat& rhs) const -> bool {
      return value() > rhs.value();
    }

    constexpr auto operator*(const Resolution& rhs) const -> uword {
      return GetNumberOfChannels() * rhs.width() * rhs.height();
    }

    friend auto operator<<(std::ostream& stream, const ImageFormat& rhs) -> std::ostream& {
      switch(rhs.value()) {
#define DEFINE_TO_STRING(Name)                    \
        case k##Name: return stream << #Name;
        FOR_EACH_IMAGE_FORMAT(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
        default:
          return stream << "Unknown ImageFormat: " << static_cast<uword>(rhs.value());
      }
    }
  };

#define DEFINE_FORMAT(Name)                                         \
  static constexpr const auto k##Name##Format = ImageFormat(ImageFormat::k##Name);
  FOR_EACH_IMAGE_FORMAT(DEFINE_FORMAT)
#undef DEFINE_FORMAT
}

#endif //PRT_IMAGE_FORMAT_H