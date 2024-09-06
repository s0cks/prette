#include "prette/image/image.h"
#include <sstream>
#include "prette/image/image_png.h"
#include "prette/image/image_jpeg.h"

namespace prt::img {
  Image::Image(const ImageFormat format, const Resolution& resolution):
    format_(format),
    resolution_(resolution) {
    memset(data(), 0, GetTotalSize());
  }

  void Image::Flip() {
    const auto channels = GetNumberOfChannels();
    const auto width = GetWidth();
    const auto height = GetHeight();
    const auto total_size = GetTotalSize();
    // NOLINTNEXTLINE
    uint8_t tmp[total_size];
    for (auto i = 0; i < width; i++) {
      for (auto j = 0; j < height; j++) {
        for(auto k = 0; k < channels; k++) {
          tmp[(i + j * width) * channels + k] = data()[(i + (height - 1 - j) * width) * channels + k];
        }
      }
    }
    memcpy(data(), &tmp[0], total_size);
  }

  auto Image::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Image(";
    ss << "format=" << GetFormat() << ", ";
    ss << "resolution=" << GetResolution() << ", ";
    ss << "data=" << (void*) data();
    ss << ")";
    return ss.str();
  }

  template<class I>
  static inline void
  Concat(ExtensionSet& extensions) {
    const auto exts = I::GetValidExtensions();
    extensions.insert(std::begin(exts), std::end(exts));
  }

  auto GetValidExtensions() -> const ExtensionSet& {
    static ExtensionSet extensions;
    if(!extensions.empty())
      return extensions;
    Concat<Png>(extensions);
    Concat<Jpeg>(extensions);
    return extensions;
  }

  auto Filter(const uri::Uri& uri) -> bool {
    return uri.HasScheme("file")
        && uri.HasExtension(GetValidExtensions());
  }

  static inline auto
  DecodeImage(const uri::Uri& uri) -> Image* {
    if(Png::Filter(uri))
      return Png::Decode(uri);
    else if(Jpeg::Filter(uri))
      return Jpeg::Decode(uri);
    LOG(ERROR) << "invalid Image uri: " << uri;
    return nullptr;
  }

  auto Decode(const uri::Uri& uri) -> Image* {
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension());
    const auto image = DecodeImage(uri);
    if(!image) {
      LOG(ERROR) << "invalid image uri: " << uri;
      return nullptr;
    }
    DLOG(INFO) << "decoded: " << image->ToString();
    image->Flip();
    return image;
  }

  auto DecodeAsync(const uri::Uri& uri) -> rx::observable<Image*> {
    return rx::observable<>::create<Image*>([uri](rx::subscriber<Image*> s) {
      const auto image = Decode(uri);
      if(!image) {
        const auto err = fmt::format("failed to decode image: {0:s}", (const std::string&) uri);
        return s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
      }
      s.on_next(image);
      s.on_completed();
    });
  }

  auto Image::operator new(const size_t sz, const uword num_bytes) -> void* {
    using namespace units::data;
    const auto total_size = sz + num_bytes;
    const auto ptr = malloc(total_size);
    // NOLINTNEXTLINE
    LOG_IF(FATAL, !ptr) << "failed to allocate data " << byte_t(total_size) << " for Image of " << byte_t(num_bytes) << " size.";
    return ptr;
  }

  void Image::operator delete(void* ptr, const size_t sz) {
    PRT_ASSERT(ptr);
    free(ptr);
  }

  auto Image::New(const ImageFormat format, const Resolution& resolution) -> Image* {
    const auto total_size = format * resolution;
    return new(total_size)Image(format, resolution);
  }
}