#include "prette/test_cache.h"

namespace prt {
  TEST_F(CacheTest, Test_Get_WillPass_UsesCacheLoader) {
    const auto k1 = "key1";
    const auto v1 = 1;

    MockCacheLoader loader;
    EXPECT_CALL(loader, Call(k1))
      .WillOnce(::testing::Return(v1));

    auto cache = Cache<std::string, uint64_t, DefaultHasher>::NewLru(10);
  }
}