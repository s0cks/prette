#include "prette/image/image.h"
#include <sstream>

namespace prt::img {
  Image::Image(const ImageFormat format, const Resolution& resolution):
    Object(),
    format_(format),
    resolution_(resolution) {
    memset(data(), 0, GetTotalSize());
  }

  Image::~Image() {
    // do nothing
  }

  void* Image::operator new(const size_t sz, const uword num_bytes) {
    using namespace units::data;
    const auto total_size = sz + num_bytes;
    const auto ptr = malloc(total_size);
    LOG_IF(FATAL, !ptr) << "failed to allocate data " << byte_t(total_size) << " for Image of " << byte_t(num_bytes) << " size.";
    return ptr;
  }

  void Image::Flip() {
    const auto channels = GetNumberOfChannels();
    const auto width = GetWidth();
    const auto height = GetHeight();
    const auto total_size = GetTotalSize();
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

  std::string Image::ToString() const {
    std::stringstream ss;
    ss << "Image(";
    ss << "format=" << GetFormat() << ", ";
    ss << "resolution=" << GetResolution() << ", ";
    ss << "data=" << (void*) data();
    ss << ")";
    return ss.str();
  }

  const std::unordered_set<std::string>&
  GetValidExtensions() {
    static std::unordered_set<std::string> extensions;
    if(!extensions.empty())
      return extensions;
    extensions.insert(std::begin(png::kValidExtensions), std::end(png::kValidExtensions));
    extensions.insert(std::begin(jpeg::kValidExtensions), std::end(jpeg::kValidExtensions));
    return extensions;
  }

  bool Filter(const uri::Uri& uri) {
    return uri.HasScheme("file")
        && uri.HasExtension(GetValidExtensions());
  }

  static inline Image*
  DecodeImage(const uri::Uri& uri) {
    if(png::Filter(uri)) {
      return png::Decode(uri);
    } else if(jpeg::Filter(uri)) {
      return jpeg::Decode(uri);
    }
    return nullptr;
  }

  Image* Decode(const uri::Uri& uri) {
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

  rx::observable<Image*> DecodeAsync(const uri::Uri& uri) {
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

  Image* Image::New(const ImageFormat format, const Resolution& resolution) {
    const auto total_size = format * resolution;
    return new(total_size)Image(format, resolution);
  }
}