#ifndef PRT_TEXTURE_FILTER_ASSERTIONS_H
#define PRT_TEXTURE_FILTER_ASSERTIONS_H

#include <gtest/gtest.h>
#include "prette/texture/texture_filter.h"

namespace prt::texture {
  using namespace ::testing;

  static inline AssertionResult
  IsTextureFilter(const TextureFilterComponent expected_min,
                  const TextureFilterComponent expected_mag,
                  const TextureFilter& actual) {
    if(actual.min != expected_min)
      return AssertionFailure() << "expected " << actual << " min component " << actual.min << " to be: " << expected_min;
    if(actual.mag != expected_mag)
      return AssertionFailure() << "expected " << actual << " mag component " << actual.mag << " to be: " << expected_mag;
    return AssertionSuccess();
  }

  static inline AssertionResult
  IsTextureFilter(const TextureFilter& expected,
                  const TextureFilter& actual) {
    return IsTextureFilter(expected.min, expected.mag, actual);
  }

  static inline AssertionResult
  IsTextureFilter(const TextureFilterComponent expected,
                  const TextureFilter& actual) {
    return IsTextureFilter(expected, expected, actual);
  }
}

#endif //PRT_TEXTURE_FILTER_ASSERTIONS_H