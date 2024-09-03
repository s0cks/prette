#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "prette/resolution.h"

namespace prt {
  using namespace ::testing;

  class ResolutionTest : public Test {
  protected:
    ResolutionTest() = default;
  public:
    ~ResolutionTest() override = default;
  };

  TEST_F(ResolutionTest, Test_0) {
    Resolution resolution("1440x1080");
    ASSERT_EQ(resolution.width(), 1440);
    ASSERT_EQ(resolution.height(), 1080);
  }
}