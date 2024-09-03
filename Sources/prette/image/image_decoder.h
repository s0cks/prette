#ifndef PRT_IMAGE_DECODER_H
#define PRT_IMAGE_DECODER_H

#include "prette/rx.h"
#include "prette/uri.h"

namespace prt::img {
  class ImageDecoder {
  protected:
    uri::Uri target_;

    explicit ImageDecoder(const uri::Uri& target):
      target_(target) {
      PRT_ASSERT(target.HasScheme("file"));
    }

    inline uri::Uri& target() {
      return target_;
    }
  public:
    virtual ~ImageDecoder() = default;
    virtual rx::observable<Image*> Decode() = 0;
  };
}

#endif //PRT_IMAGE_DECODER_H