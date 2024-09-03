#include <gtest/gtest.h>

#include "prette/color.h"

namespace prt {
  using namespace ::testing;
  using namespace rgb;

  class ColorTest : public Test {
  protected:
    ColorTest() = default;
  public:
    ~ColorTest() override = default;
  };

  template<const ColorComponent Component>
  static inline AssertionResult
  HasComponent(const Color& actual, const float expected) {
    return actual[Component] == expected
         ? AssertionSuccess()
         : AssertionFailure() << "expected " << glm::to_string(actual) << " to have a " << Component << " component of: " << expected << ", not: " << actual[Component];
  }

  static inline AssertionResult
  HasRedComponent(const Color& actual, const float expected) {
    return HasComponent<kRedComponent>(actual, expected);
  }

  static inline AssertionResult
  HasBlueComponent(const Color& actual, const float expected) {
    return HasComponent<kBlueComponent>(actual, expected);
  }

  static inline AssertionResult
  HasGreenComponent(const Color& actual, const float expected) {
    return HasComponent<kGreenComponent>(actual, expected);
  }

  static inline AssertionResult
  IsColor(const Color& actual, const float r, const float g, const float b) {
    {
      const auto result = HasRedComponent(actual, r);
      if(!result)
        return result;
    }
    {
      const auto result = HasGreenComponent(actual, g);
      if(!result)
        return result;
    }
    {
      const auto result = HasBlueComponent(actual, b);
      if(!result)
        return result;
    }
    return AssertionSuccess();
  }

  TEST_F(ColorTest, Test_Black) {
    ASSERT_TRUE(IsColor(kBlack, 0.0f, 0.0f, 0.0f));
  }

  TEST_F(ColorTest, Test_White) {
    ASSERT_TRUE(IsColor(kWhite, 1.0f, 1.0f, 1.0f));
  }

  TEST_F(ColorTest, Test_Red) {
    ASSERT_TRUE(IsColor(kRed, 1.0f, 0.0f, 0.0f));
  }

  TEST_F(ColorTest, Test_Green) {
    ASSERT_TRUE(IsColor(kGreen, 0.0f, 1.0f, 0.0f));
  }

  TEST_F(ColorTest, Test_Blue) {
    ASSERT_TRUE(IsColor(kBlue, 0.0f, 0.0f, 1.0f));
  }
}