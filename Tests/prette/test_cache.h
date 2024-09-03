#ifndef PRT_TEST_CACHE_H
#define PRT_TEST_CACHE_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "prette/cache.h"

namespace prt {
  class CacheTest : public ::testing::Test {
  protected:
    CacheTest() = default;
  public:
    ~CacheTest() override = default;
  };

  typedef ::testing::MockFunction<uint64_t(const std::string& key)> MockCacheLoader;
}

#endif //PRT_TEST_CACHE_H