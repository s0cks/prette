#include "prette/properties/property_test.h"

namespace prt::properties {
  class DoublePropertyTest : public PropertyTest {
  protected:
    DoublePropertyTest() = default;
  public:
    ~DoublePropertyTest() override = default;
  };

  TEST_F(DoublePropertyTest, Test_New) {
    static constexpr const auto kPropertyName = "test";
    static const double kPropertyValue = 3.141592654;
    const auto property = DoubleProperty::New(kPropertyName, kPropertyValue);
    ASSERT_TRUE(property);
    ASSERT_TRUE(IsDoubleProperty(property));
    ASSERT_TRUE(HasName(property, kPropertyName));
    ASSERT_TRUE(HasValue(property, kPropertyValue));
  }
}