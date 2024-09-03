#ifndef PRT_IMAGE_H
#define PRT_IMAGE_H

#include "prette/gfx.h"
#include "prette/uri.h"
#include "prette/object.h"
#include "prette/image/image_format.h"

namespace prt::img {
  const std::unordered_set<std::string>& GetValidExtensions();

  typedef glm::u32vec2 ImageSize;

  class Image : public Object {
    DEFINE_NON_COPYABLE_TYPE(Image);
  private:
    static inline uword
    GetFormatOffset() {
      return sizeof(Image);
    }

    static inline uword
    GetSizeOffset() {
      return GetFormatOffset() + sizeof(uword);
    }

    static inline uword
    GetDataOffset() {
      return GetSizeOffset() + sizeof(uword);
    }

    static inline uword
    CalculateAllocationSize(const uword num_bytes) {
      uword sz = 0;
      sz += sizeof(Image);
      sz += sizeof(uword); // format
      sz += (sizeof(uword) * 2); // size
      sz += (sizeof(uint8_t) * num_bytes); // data
      return sz;
    }

    static inline uword
    CalculateNumberOfBytes(const ImageFormat format, const ImageSize& size) {
      switch(format) {
        case kRGB:
          return 3 * size[0] * size[1];
        case kRGBA:
          return 4 * size[0] * size[1];
        default:
          return 0;
      }
    }

    static inline uword
    CalculateAllocationSize(const ImageFormat format, const ImageSize& size) {
      return CalculateAllocationSize(CalculateNumberOfBytes(format, size));
    }
  protected:
    Image(const ImageFormat format,
          const ImageSize& size):
      Object() {
      SetFormat(format);
      SetSize(size);
    }

    inline uword raw_ptr() const {
      return (uword)this;
    }

    inline ImageFormat* format_ptr() const {
      return (ImageFormat*) (raw_ptr() + GetFormatOffset());
    }

    inline void SetFormat(const ImageFormat& rhs) {
      (*format_ptr()) = rhs;
    }

    inline ImageSize* size_ptr() const {
      return (ImageSize*) (raw_ptr() + GetSizeOffset());
    }

    inline void SetSize(const ImageSize& rhs) {
      (*size_ptr()) = rhs;
    }

    inline uint8_t* data_ptr() const {
      return (uint8_t*) (raw_ptr() + GetDataOffset());
    }

    inline void ClearData() {
      NOT_IMPLEMENTED(FATAL); //TODO: implement
    }
  public:
    ~Image() override = default;
    std::string ToString() const override;

    virtual ImageFormat format() const {
      return *format_ptr();
    }

    inline int GetNumberOfColorComponents() const {
      switch(format()) {
        case kRGBA:
          return 4;
        case kRGB:
        default:
          return 3;
      }
    }

    const ImageSize& size() const {
      return *size_ptr();
    }

    uword width() const {
      return (*size_ptr())[0];
    }

    uword height() const {
      return (*size_ptr())[1];
    }

    uint8_t* data() const {
      return data_ptr();
    }

    void Flip();
    uword GetNumberOfBytes() const;
    uword GetTotalNumberOfBytes() const;
  public:
    void* operator new(const size_t sz, const uword num_bytes);
    static Image* New(const ImageFormat format, const ImageSize& size, const uword num_bytes);
  };

  bool Filter(const uri::Uri& uri);
  Image* Decode(const uri::Uri& uri);
  rx::observable<Image*> DecodeAsync(const uri::Uri& uri);
}

#include "prette/image/image_png.h"
#include "prette/image/image_jpeg.h"

#endif //PRT_IMAGE_H