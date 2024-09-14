#include <gtest/gtest.h>

#include "prette/mock_glfw.h"
#include "prette/keyboard/key.h"

namespace prt::keyboard {
  using namespace ::testing;

  class KeyTest : public Test {
  protected:
    KeyTest() = default;
  public:
    ~KeyTest() override = default;
  };

  TEST_F(KeyTest, Test_Constructor) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Key k1;
    ASSERT_TRUE(k1.IsUnknown());

    Key k2(GLFW_KEY_A);
    ASSERT_TRUE(k2.IsCode(GLFW_KEY_A));
  }

  TEST_F(KeyTest, Test_Equals) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Key k1;
    Key k2;
    ASSERT_EQ(k1, k2);

    Key k3(GLFW_KEY_A);
    Key k4(GLFW_KEY_A);
    ASSERT_EQ(k3, k4);

    ASSERT_NE(k1, k3);
    ASSERT_NE(k2, k4);
  }

  class KeySetTest : public Test {
  protected:
    KeySetTest() = default;
  public:
    ~KeySetTest() override = default;
  };

  TEST_F(KeySetTest, Test_Constructor) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    MockGlfw glfw;
    EXPECT_CALL(glfw, glfwGetKeyName(_, _))
      .WillRepeatedly(Return((const char*) nullptr));

    KeySet keys;
    DLOG(INFO) << "Keys:";
    for(const auto& key : keys) {
      DLOG(INFO) << " - " << key;
    }
  }
}