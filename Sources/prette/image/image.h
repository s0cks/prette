#ifndef PRT_IMAGE_H
#define PRT_IMAGE_H

#include "prette/gfx.h"
#include "prette/uri.h"
#include "prette/object.h"
#include "prette/resolution.h"
#include "prette/image/image_format.h"

namespace prt::img {
  const std::unordered_set<std::string>& GetValidExtensions();

  class Image : public Object {
    DEFINE_NON_COPYABLE_TYPE(Image);
  private:
    ImageFormat format_;
    Resolution resolution_;

    Image(const ImageFormat format, const Resolution& resolution);
  public:
    ~Image() override;

    ImageFormat GetFormat() const {
      return format_;
    }

    const Resolution& GetResolution() const {
      return resolution_;
    }

    inline uword GetWidth() const {
      return static_cast<uword>(GetResolution().width());
    }

    inline uword GetHeight() const {
      return static_cast<uword>(GetResolution().height());
    }

    inline uword GetArea() const {
      return GetWidth() * GetHeight();
    }

    inline uword GetNumberOfChannels() const {
      return GetFormat().GetNumberOfChannels();
    }

    inline uword GetTotalSize() const {
      return GetArea() * GetNumberOfChannels();
    }
    
    uint8_t* data() const {
      return (uint8_t*) (((uword)this) + sizeof(Image));
    }

    std::string ToString() const override;
    void Flip(); //TODO: reduce visibility
  public:
    void* operator new(const size_t sz, const uword num_bytes);
    static Image* New(const ImageFormat format, const Resolution& resolution);
  };
  
  bool Filter(const uri::Uri& uri);
  Image* Decode(const uri::Uri& uri);
  rx::observable<Image*> DecodeAsync(const uri::Uri& uri);
}

#include "prette/image/image_png.h"
#include "prette/image/image_jpeg.h"

#endif //PRT_IMAGE_H