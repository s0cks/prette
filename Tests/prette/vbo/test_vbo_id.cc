#include <gtest/gtest.h>
#include "prette/vbo/vbo_id.h"

namespace prt::vbo {
  using namespace ::testing;

  class VboIdTest : public Test {
  protected:
    VboIdTest() = default;
  public:
    ~VboIdTest() override = default;
  };

  TEST_F(VboIdTest, Test_Size) {
    ASSERT_EQ(sizeof(VboId), sizeof(GLuint));
  }

  TEST_F(VboIdTest, Test_IsValid) {
    static constexpr const VboId kId1 = 0;
    ASSERT_TRUE(kId1.IsValid());
    static constexpr const VboId kId2 = 1;
    ASSERT_TRUE(kId2.IsValid());
  }
}