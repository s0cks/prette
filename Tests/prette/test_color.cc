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
  static inline auto
  HasComponent(const Color& actual, const float expected) -> AssertionResult {
    return (actual[Component] / 255.0f) == expected
         ? AssertionSuccess()
         : AssertionFailure() << "expected " << glm::to_string(actual) << " to have a " << Component << " component of: " << expected << ", not: " << actual[Component];
  }

  static inline auto
  HasRedComponent(const Color& actual, const float expected) -> AssertionResult {
    return HasComponent<kRedComponent>(actual, expected);
  }

  static inline auto
  HasBlueComponent(const Color& actual, const float expected) -> AssertionResult {
    return HasComponent<kBlueComponent>(actual, expected);
  }

  static inline auto
  HasGreenComponent(const Color& actual, const float expected) -> AssertionResult {
    return HasComponent<kGreenComponent>(actual, expected);
  }

  static inline auto
  IsColor(const Color& actual, const float r, const float g, const float b) -> AssertionResult {
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

  TEST_F(ColorTest, Test_Black) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    ASSERT_TRUE(IsColor(kBlack, 0.0f, 0.0f, 0.0f));
  }

  TEST_F(ColorTest, Test_White) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    ASSERT_TRUE(IsColor(kWhite, 1.0f, 1.0f, 1.0f));
  }

  TEST_F(ColorTest, Test_Red) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    ASSERT_TRUE(IsColor(kRed, 1.0f, 0.0f, 0.0f));
  }

  TEST_F(ColorTest, Test_Green) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    ASSERT_TRUE(IsColor(kGreen, 0.0f, 1.0f, 0.0f));
  }

  TEST_F(ColorTest, Test_Blue) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    ASSERT_TRUE(IsColor(kBlue, 0.0f, 0.0f, 1.0f));
  }
}