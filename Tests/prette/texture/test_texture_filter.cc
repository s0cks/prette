#include <gtest/gtest.h>

#include "prette/mock_gl.h"
#include "prette/texture/texture_filter.h"

namespace prt::texture {
  class TextureFilterTest : public ::testing::Test {
  protected:
    TextureFilterTest() = default;
  public:
    ~TextureFilterTest() override = default;
  };

  TEST_F(TextureFilterTest, Test_ApplyTo_WillPass) {
    static const auto target = k2D;
    static const auto filter = kLinearFilter;

    MockGl gl;
    EXPECT_CALL(gl, glTexParameteri(::testing::Eq(target), ::testing::Eq(GL_TEXTURE_MIN_FILTER), ::testing::Eq(filter.min)));
    EXPECT_CALL(gl, glTexParameteri(::testing::Eq(target), ::testing::Eq(GL_TEXTURE_MAG_FILTER), ::testing::Eq(filter.mag)));
    filter.ApplyTo(target);
  }

  TEST_F(TextureFilterTest, TestParseTextureFilterComponent_WillPass_NearestMipmapNearest) {
    const auto component = ParseTextureFilterComponent("NearestMipmapNearest");
    ASSERT_TRUE(component);
    ASSERT_EQ((*component), kNearestMipmapNearest);
  }

  TEST_F(TextureFilterTest, TestParseTextureFilterComponent_WillPass_NearestMipmapLinear) {
    const auto component = ParseTextureFilterComponent("NearestMipmapLinear");
    ASSERT_TRUE(component);
    ASSERT_EQ((*component), kNearestMipmapLinear);
  }

  TEST_F(TextureFilterTest, TestParseTextureFilterComponent_WillPass_LinearMipmapNearest) {
    const auto component = ParseTextureFilterComponent("LinearMipmapNearest");
    ASSERT_TRUE(component);
    ASSERT_EQ((*component), kLinearMipmapNearest);
  }

  TEST_F(TextureFilterTest, TestParseTextureFilterComponent_WillPass_LinearMipmapLinear) {
    const auto component = ParseTextureFilterComponent("LinearMipmapLinear");
    ASSERT_TRUE(component);
    ASSERT_EQ((*component), kLinearMipmapLinear);
  }

  TEST_F(TextureFilterTest, TestParseTextureFilterComponent_WillPass_Linear) {
    const auto component = ParseTextureFilterComponent("Linear");
    ASSERT_TRUE(component);
    ASSERT_EQ((*component), kLinear);
  }

  TEST_F(TextureFilterTest, TestParseTextureFilterComponent_WillPass_Nearest) {
    const auto component = ParseTextureFilterComponent("Nearest");
    ASSERT_TRUE(component);
    ASSERT_EQ((*component), kNearest);
  }

  TEST_F(TextureFilterTest, TestParseTextureFilterComponent_WillPass_Default) {
    const auto component = ParseTextureFilterComponent("Default");
    ASSERT_TRUE(component);
    ASSERT_EQ((*component), kLinear);
  }

  class JsonTextureFilterTest : public ::testing::Test {
  protected:
    JsonTextureFilterTest() = default;
  public:
    ~JsonTextureFilterTest() override = default;
  };

  static inline ::testing::AssertionResult
  IsTextureFilter(const TextureFilter& actual, const TextureFilterComponent min, const TextureFilterComponent mag) {
    if(actual.min != min)
      return ::testing::AssertionFailure() << "expected " << actual << ".min to be: " << min;
    if(actual.mag != mag)
      return ::testing::AssertionFailure() << "expected " << actual << ".mag to be: " << mag;
    return ::testing::AssertionSuccess();
  }

  static inline ::testing::AssertionResult
  IsTextureFilter(const TextureFilter& actual, const TextureFilterComponent expected) {
    return IsTextureFilter(actual, expected, expected);
  }

  static inline ::testing::AssertionResult
  IsTextureFilter(const TextureFilter& actual, const TextureFilter& expected) {
    return IsTextureFilter(actual, expected.min, expected.mag);
  }

  TEST_F(JsonTextureFilterTest, Test_StringEqualsLinear) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("\"linear\"", doc));
    ASSERT_TRUE(doc.IsString());
    const auto filter = TextureFilter(std::string(doc.GetString(), doc.GetStringLength()));
    ASSERT_TRUE(IsTextureFilter((const TextureFilter) filter, kLinear));
  }

  TEST_F(JsonTextureFilterTest, Test_StringEqualsNearest) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("\"nearest\"", doc));
    ASSERT_TRUE(doc.IsString());
    const auto filter = TextureFilter(std::string(doc.GetString(), doc.GetStringLength()));
    ASSERT_TRUE(IsTextureFilter((const TextureFilter) filter, kNearest));
  }

  TEST_F(JsonTextureFilterTest, Test_MinEqualsNearestMagEqualsLinear) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("{ \"min\": \"nearest\", \"mag\": \"linear\" }", doc));
    const auto filter = TextureFilter(doc.GetObject());
    ASSERT_TRUE(IsTextureFilter(filter, kNearest, kLinear));
  }
}