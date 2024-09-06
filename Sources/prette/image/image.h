#ifndef PRT_IMAGE_H
#define PRT_IMAGE_H

#include "prette/uri.h"
#include "prette/resolution.h"
#include "prette/image/image_format.h"

namespace prt::img {
  using uri::ExtensionSet;

  auto GetValidExtensions() -> const ExtensionSet&;

  class Image {
    DEFINE_NON_COPYABLE_TYPE(Image);
  private:
    ImageFormat format_;
    Resolution resolution_;

    Image(const ImageFormat format, const Resolution& resolution);
  public:
    ~Image() = delete;

    auto GetFormat() const -> ImageFormat {
      return format_;
    }

    auto GetResolution() const -> const Resolution& {
      return resolution_;
    }

    inline auto GetWidth() const -> uword {
      return static_cast<uword>(GetResolution().width());
    }

    inline auto GetHeight() const -> uword {
      return static_cast<uword>(GetResolution().height());
    }

    inline auto GetArea() const -> uword {
      return GetWidth() * GetHeight();
    }

    inline auto GetNumberOfChannels() const -> uword {
      return GetFormat().GetNumberOfChannels();
    }

    inline auto GetTotalSize() const -> uword {
      return GetArea() * GetNumberOfChannels();
    }
    
    auto data() const -> uint8_t* {
      // NOLINTNEXTLINE
      return (uint8_t*) (((uword)this) + sizeof(Image));
    }

    auto ToString() const -> std::string;
    void Flip(); //TODO: reduce visibility
  public:
    void operator delete(void* ptr) = delete;
    void operator delete(void* ptr, const size_t sz);
    auto operator new(const size_t sz, const uword num_bytes) -> void*;
    auto operator new(const size_t sz) -> void* = delete;

    void operator delete[](void* ptr) = delete;
    void operator delete[](void* ptr, const uword num_bytes) = delete;
    auto operator new[](const size_t sz, const uword num_bytes) -> void* = delete;
    auto operator new[](const size_t sz) -> void* = delete;

    static auto New(const ImageFormat format, const Resolution& resolution) -> Image*;
  };
  
  auto Filter(const uri::Uri& uri) -> bool;
  auto Decode(const uri::Uri& uri) -> Image*;
  auto DecodeAsync(const uri::Uri& uri) -> rx::observable<Image*>;
}

#endif //PRT_IMAGE_H