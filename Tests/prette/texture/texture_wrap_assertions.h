#ifndef PRT_TEXTURE_WRAP_ASSERTIONS_H
#define PRT_TEXTURE_WRAP_ASSERTIONS_H

#include <gtest/gtest.h>

#include "prette/texture/texture_wrap.h"

namespace prt::texture {
  using namespace ::testing;

  static inline AssertionResult
  IsTextureWrap(const TextureWrapMode r,
                const TextureWrapMode s,
                const TextureWrapMode t,
                const TextureWrap& actual) {
    if(actual.r != r)
      return AssertionFailure() << "expected actual.r " << actual.r << " to equal: " << r;
    if(actual.s != s)
      return AssertionFailure() << "expected actual.s " << actual.s << " to equal: " << s;
    if(actual.t != t)
      return AssertionFailure() << "expected actual.t " << actual.t << " to equal: " << t;
    return AssertionSuccess();
  }

  static inline AssertionResult
  IsTextureWrap(const TextureWrapMode mode,
                const TextureWrap& actual) {
    return IsTextureWrap(mode, mode, mode, actual);
  }
}

#endif //PRT_TEXTURE_WRAP_ASSERTIONS_H