#ifndef PRT_RBO_SIZE_H
#define PRT_RBO_SIZE_H

#include "prette/gfx.h"

namespace prt::rbo {
  typedef glm::i32vec2 RboSize;

  int32_t GetMaxRboSize();

  static inline bool
  IsValidRboSize(const RboSize& size) {
    const auto max = GetMaxRboSize();
    return size[0] >= 0
        && size[0] <= max
        && size[1] >= 0
        && size[1] <= max;
  }
}

#endif //PRT_RBO_SIZE_H