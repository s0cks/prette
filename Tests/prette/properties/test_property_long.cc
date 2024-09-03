#include "prette/properties/property_test.h"

namespace prt::properties {
  class LongPropertyTest : public PropertyTest {
  protected:
    LongPropertyTest() = default;
  public:
    ~LongPropertyTest() override = default;
  };

  TEST_F(LongPropertyTest, Test_New) {
    static constexpr const auto kPropertyName = "test";
    static constexpr const uint64_t kPropertyValue = 10;
    const auto prop = LongProperty::New(kPropertyName, kPropertyValue);
    ASSERT_TRUE(prop);
    ASSERT_TRUE(IsLongProperty(prop));
    ASSERT_TRUE(HasName(prop, kPropertyName));
    ASSERT_TRUE(HasValue(prop, kPropertyValue));
  }
}