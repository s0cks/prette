#include "prette/properties/property_test.h"

namespace prt::properties {
  class StringPropertyTest : public PropertyTest {
  protected:
    StringPropertyTest() = default;
  public:
    ~StringPropertyTest() override = default;
  };

  TEST_F(StringPropertyTest, Test_New) {
    static constexpr const auto kPropertyName = "test";
    static const std::string kPropertyValue = "value";
    const auto property = StringProperty::New(kPropertyName, kPropertyValue);
    ASSERT_TRUE(property);
    ASSERT_TRUE(IsStringProperty(property));
    ASSERT_TRUE(HasName(property, kPropertyName));
    ASSERT_TRUE(HasValue(property, kPropertyValue));
  }
}