#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "prette/resolution.h"

namespace prt {
using namespace ::testing;

class ResolutionTest : public Test {
 protected:
  ResolutionTest() = default;

 public:
  ~ResolutionTest() override = default;
};

TEST_F(ResolutionTest, Test_0) {  // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
  Resolution resolution("1440x1080");
  ASSERT_EQ(resolution.GetWidth(), 1440);
  ASSERT_EQ(resolution.GetHeight(), 1080);
}
}  // namespace prt