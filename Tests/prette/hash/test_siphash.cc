#include <gtest/gtest.h>
#include "prette/hash/siphash.h"

namespace prt {
  using namespace ::testing;

  class SipHashTest : public Test {
  protected:
    SipHashTest() = default;
  public:
    ~SipHashTest() override = default;
  };

  TEST_F(SipHashTest, Test1) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const uword kExpectedHash = 0x3d1974e948748ce2;
    const auto key = std::array<uint8_t, 16>{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    ASSERT_EQ(kExpectedHash, siphash24::of("hello", key));
  }

  TEST_F(SipHashTest, Test2) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const uword kExpectedHash = 14986662229302055855ul;
    const auto key = std::array<uint8_t, 16>{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    ASSERT_EQ(kExpectedHash, siphash48::of("hello", key));
  }
}