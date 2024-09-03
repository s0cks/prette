#include <gtest/gtest.h>

#include "prette/texture/texture_json.h"
#include "prette/texture/texture_wrap_assertions.h"
#include "prette/texture/texture_filter_assertions.h"

namespace prt::texture {
  using namespace ::testing;

  class TextureValueTest : public Test {
  protected:
    TextureValueTest() = default;
  public:
    ~TextureValueTest() override = default;
  };

  TEST_F(TextureValueTest, Test1) {
    static constexpr const auto kTestJson = "{\"data\": { } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    ASSERT_FALSE(value.GetFileProperty());
    ASSERT_FALSE(value.GetWrapProperty());
    ASSERT_FALSE(value.GetFilterProperty());
  }

  TEST_F(TextureValueTest, Test_Filter_String_Linear) {
    static constexpr const auto kTestJson = "{\"data\": { \"filter\": \"linear\" } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    const auto filter = value.GetFilter();
    ASSERT_TRUE(filter);
    ASSERT_TRUE(IsTextureFilter(kLinear, *filter));
  }

  TEST_F(TextureValueTest, Test_Filter_String_Nearest) {
    static constexpr const auto kTestJson = "{\"data\": { \"filter\": \"nearest\" } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    const auto filter = value.GetFilter();
    ASSERT_TRUE(filter);
    ASSERT_TRUE(IsTextureFilter(kNearest, *filter));
  }

  TEST_F(TextureValueTest, Test_Filter_String_Default) {
    static constexpr const auto kTestJson = "{\"data\": { \"filter\": \"default\" } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    const auto filter = value.GetFilter();
    ASSERT_TRUE(filter);
    ASSERT_TRUE(IsTextureFilter(kLinear, *filter));
  }

  TEST_F(TextureValueTest, Test_Filter_Object_LinearNearest) {
    static constexpr const auto kTestJson = "{\"data\": { \"filter\": { \"min\": \"linear\", \"mag\": \"nearest\" } } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    const auto filter = value.GetFilter();
    ASSERT_TRUE(filter);
    ASSERT_TRUE(IsTextureFilter(kLinear, kNearest, *filter));
  }

  TEST_F(TextureValueTest, Test_Wrap_String_ClampToEdge) {
    static constexpr const auto kTestJson = "{\"data\": { \"wrap\": \"clamp2edge\" } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    const auto wrap = value.GetWrap();
    ASSERT_TRUE(wrap);
    ASSERT_TRUE(IsTextureWrap(kClampToEdge, *wrap));
  }

  TEST_F(TextureValueTest, Test_Wrap_String_Default) {
    static constexpr const auto kTestJson = "{\"data\": { \"wrap\": \"default\" } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    const auto wrap = value.GetWrap();
    ASSERT_TRUE(wrap);
    ASSERT_TRUE(IsTextureWrap(kClampToEdge, *wrap));
  }

  TEST_F(TextureValueTest, Test_Wrap_Object_RepeatClampToEdgeRepeat) {
    static constexpr const auto kTestJson = "{\"data\": { \"wrap\": { \"r\": \"repeat\", \"s\": \"clamp2edge\", \"t\": \"repeat\" } } }";

    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson(kTestJson, doc));

    json::TextureValue value(&doc["data"]);
    const auto wrap = value.GetWrap();
    ASSERT_TRUE(wrap);
    ASSERT_TRUE(IsTextureWrap(kRepeat, kClampToEdge, kRepeat, *wrap));
  }
}