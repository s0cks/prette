#ifndef PRT_TEXTURE_DATA_H
#define PRT_TEXTURE_DATA_H

#include <ostream>
#include <units.h>

#include "prette/glm.h"
#include "prette/common.h"
#include "prette/image/image.h"
#include "prette/texture/texture_format.h"

namespace prt::texture {
  class TextureData {
    friend class TextureBuilder;
    DEFINE_DEFAULT_COPYABLE_TYPE(TextureData);
    static constexpr const GLenum kDefaultType = GL_UNSIGNED_BYTE;
  protected:
    GLenum type_;
    TextureFormat format_;
    TextureFormat internal_format_;
    const uint8_t* bytes_;
    glm::vec2 size_;
    int level_;
    int border_;
  public:
    TextureData(const GLenum type,
                const TextureFormat format,
                const uint8_t* bytes,
                const glm::vec2& size, 
                const int level = 0,
                const int border = 0):
      type_(type),
      format_(format),
      internal_format_(format),
      bytes_(bytes),
      size_(size),
      level_(level),
      border_(border) {
    }
    explicit TextureData(const glm::vec2& size,
                         const GLenum type = kDefaultType,
                         const TextureFormat format = kDefaultTextureFormat,
                         const int level = 0,
                         const int border = 0):
      TextureData(type, format, NULL, size, level, border) {
    }
    TextureData():
      TextureData(glm::vec2(0)) {
    }
    ~TextureData() = default;

    GLenum type() const {
      return type_;
    }

    TextureFormat format() const {
      return format_;
    }

    TextureFormat internal_format() const {
      return internal_format_;
    }

    const uint8_t* bytes() const {
      return bytes_;
    }

    int level() const {
      return level_;
    }

    int border() const {
      return border_;
    }

    const glm::vec2& size() const {
      return size_;
    }

    uint64_t width() const {
      return size_[0];
    }

    uint64_t height() const {
      return size_[1];
    }

    uint64_t GetNumberOfBytes() const;

    void operator=(const img::Image* image);

    friend std::ostream& operator<<(std::ostream& stream, const TextureData& rhs) {
      using namespace units::data;
      stream << "TextureData(";
      stream << "format=" << rhs.format() << ", ";
      stream << "internal_format=" << rhs.internal_format() << ", ";
      stream << "type=" << rhs.type() << ", ";
      stream << "bytes=" << rhs.bytes() << ", ";
      stream << "num_bytes=" << byte_t(rhs.GetNumberOfBytes()) << ", ";
      stream << "size=" << glm::to_string(rhs.size());
      stream << ")";
      return stream;
    }

    bool operator==(const TextureData& rhs) const;
    bool operator!=(const TextureData& rhs) const;
  };
}

#endif //PRT_TEXTURE_DATA_H