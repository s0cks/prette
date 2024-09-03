#include <gtest/gtest.h>
#include "prette/json_schema.h"

namespace prt {
  using namespace ::testing;

  class ShaderJsonTest : public Test {
  protected:
    ShaderJsonTest() = default;
  public:
    ~ShaderJsonTest() override = default;
  };

  static inline AssertionResult
  IsValid(const json::Schema::ValidationResult& result) {
    if(result)
      return AssertionSuccess();
    return AssertionFailure() << "expected " << result << " to be valid.";
  }

  static inline AssertionResult
  IsValid(const json::Document& doc) {
    const auto schema = json::GetSchema();
    if(!schema)
      return AssertionSuccess() << "schema is null.";
    const auto result = schema->Validate(doc);
    return IsValid(schema->Validate(doc));
  }

  TEST_F(ShaderJsonTest, Test_Invalid1) {
    json::Document doc;
    ASSERT_TRUE(json::ParseJson(uri::Uri(fmt::format("file://{0:s}/shaders/invalid.shader.json", FLAGS_resources)), doc));
    ASSERT_FALSE(IsValid(doc));
  }
}