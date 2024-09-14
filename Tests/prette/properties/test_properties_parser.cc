#include <gtest/gtest.h>
#include <fmt/format.h>

#include "prette/flags.h"
#include "prette/properties/properties_parser.h"
#include "prette/properties/property_assertions.h"

namespace prt::properties {
  using namespace ::testing;

  class PropertiesParserTest : public Test {
  protected:
    PropertiesParserTest() = default;
  public:
    ~PropertiesParserTest() override = default;
  };

  TEST_F(PropertiesParserTest, Test_Parse) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kTestPropertiesFileName = "test1.properties";

    const auto filename = fmt::format("{0:s}/{1:s}", FLAGS_resources, kTestPropertiesFileName);
    PRT_ASSERT(FileExists(filename));
    const auto file = fopen(filename.c_str(), "rb");
    Parser parser(file);

    PropertySet properties;
    ASSERT_TRUE(parser.ParseProperties(properties));

    const auto& k1 = properties["k1"];
    PRT_ASSERT(k1);
    ASSERT_TRUE(IsStringProperty(k1));
    ASSERT_TRUE(HasName(k1, "k1"));
    ASSERT_TRUE(HasValue(k1, (const std::string&) "Hello World"));

    const auto& k2 = properties["k2"];
    PRT_ASSERT(k2);
    ASSERT_TRUE(IsStringProperty(k2));
    ASSERT_TRUE(HasName(k2, "k2"));
    ASSERT_TRUE(HasValue(k2, (const std::string&) "Hello World"));

    const auto& k3 = properties["k3"];
    PRT_ASSERT(k3);
    ASSERT_TRUE(IsLongProperty(k3));
    ASSERT_TRUE(HasName(k3, "k3"));
    ASSERT_TRUE(HasValue(k3, (uint64_t) 10));

    const auto& k4 = properties["k4"];
    PRT_ASSERT(k4);
    ASSERT_TRUE(IsBoolProperty(k4));
    ASSERT_TRUE(HasName(k4, "k4"));
    ASSERT_TRUE(HasValue(k4, (bool) true));

    const auto& k5 = properties["k5"];
    PRT_ASSERT(k5);
    ASSERT_TRUE(IsBoolProperty(k5));
    ASSERT_TRUE(HasName(k5, "k5"));
    ASSERT_TRUE(HasValue(k5, (bool) false));

    const auto& k6 = properties["k6"];
    PRT_ASSERT(k6);
    ASSERT_TRUE(IsDoubleProperty(k6));
    ASSERT_TRUE(HasName(k6, "k6"));
    ASSERT_TRUE(HasValue(k6, (double) 3.14159264));
  }
}