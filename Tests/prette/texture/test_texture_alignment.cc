#include <gtest/gtest.h>

#include "prette/mock_gl.h"
#include "prette/texture/texture_alignment.h"

namespace prt::texture {
  class TextureAlignmentTest : public ::testing::Test {
  protected:
    TextureAlignmentTest() = default;
  public:
    ~TextureAlignmentTest() override = default;
  };

  TEST_F(TextureAlignmentTest, Test_Apply_WillPass) {
    static const auto alignment = kDefaultAlignment;
    MockGl gl;
    EXPECT_CALL(gl, glPixelStorei(::testing::Eq(GL_PACK_ALIGNMENT), ::testing::Eq(alignment.pack)));
    EXPECT_CALL(gl, glPixelStorei(::testing::Eq(GL_UNPACK_ALIGNMENT), ::testing::Eq(alignment.unpack)));
    alignment.Apply();
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_WillFail_EmptyValue) {
    const auto alignment = ParseTextureAlignment("");
    ASSERT_FALSE(alignment);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_WillFail_UnknownValue) {
    const auto alignment = ParseTextureAlignment("thisisaninvalidtexturealignment");
    ASSERT_FALSE(alignment);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_1) {
    const auto alignment = ParseTextureAlignment("1");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), k1);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_2) {
    const auto alignment = ParseTextureAlignment("2");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), k2);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_4) {
    const auto alignment = ParseTextureAlignment("4");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), k4);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_8) {
    const auto alignment = ParseTextureAlignment("8");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), k8);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_Byte) {
    const auto alignment = ParseTextureAlignment("byte");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), kByteAlignment);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_Row) {
    const auto alignment = ParseTextureAlignment("row");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), kRowAlignment);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_Word) {
    const auto alignment = ParseTextureAlignment("word");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), kWordAlignment);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_DWord) {
    const auto alignment = ParseTextureAlignment("dword");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), kDoubleWordAlignment);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_DoubleWord) {
    const auto alignment = ParseTextureAlignment("doubleword");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), kDoubleWordAlignment);
  }

  TEST_F(TextureAlignmentTest, Test_ParseTextureAlignment_Default) {
    const auto alignment = ParseTextureAlignment("default");
    ASSERT_TRUE(alignment);
    ASSERT_EQ((*alignment), k4);
  }

  class JsonTextureAlignmentTest : public ::testing::Test {
  protected:
    JsonTextureAlignmentTest() = default;
  public:
    ~JsonTextureAlignmentTest() override = default;
  };

  static inline ::testing::AssertionResult
  IsPixelStoreAlignment(const PixelStoreAlignment& actual, const TextureAlignment pack, const TextureAlignment unpack) {
    if(actual.pack != pack)
      return ::testing::AssertionFailure() << "expected " << actual << ".pack to be: " << pack;
    if(actual.unpack != unpack)
      return ::testing::AssertionFailure() << "expected " << actual << ".unpack to be: " << unpack;
    return ::testing::AssertionSuccess();
  }

  static inline ::testing::AssertionResult
  IsPixelStoreAlignment(const PixelStoreAlignment& actual, const TextureAlignment rhs) {
    return IsPixelStoreAlignment(actual, rhs, rhs);
  }

  static inline ::testing::AssertionResult
  IsPixelStoreAlignment(const PixelStoreAlignment& actual, const PixelStoreAlignment& rhs) {
    return IsPixelStoreAlignment(actual, rhs.pack, rhs.unpack);
  }

  TEST_F(JsonTextureAlignmentTest, Test_StringIsInvalid) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("\"thisisinvalid\"", doc));
    JsonPixelStoreAlignment alignment(doc);
    ASSERT_TRUE(IsPixelStoreAlignment((const PixelStoreAlignment&) alignment, k4));
  }

  TEST_F(JsonTextureAlignmentTest, Test_StringIsEmpty) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("\"\"", doc));
    JsonPixelStoreAlignment alignment(doc);
    ASSERT_TRUE(IsPixelStoreAlignment((const PixelStoreAlignment&) alignment, k4));
  }

  TEST_F(JsonTextureAlignmentTest, Test_StringIsDoubleword) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("\"doubleword\"", doc));
    JsonPixelStoreAlignment alignment(doc);
    ASSERT_TRUE(IsPixelStoreAlignment((const PixelStoreAlignment&) alignment, kDoubleWordAlignment));
  }

  TEST_F(JsonTextureAlignmentTest, Test_NumberIs8) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("8", doc));
    JsonPixelStoreAlignment alignment(doc);
    ASSERT_TRUE(IsPixelStoreAlignment((const PixelStoreAlignment&) alignment, k8));
  }

  TEST_F(JsonTextureAlignmentTest, Test_PackIs4UnpackIsDoubleword) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("{ \"pack\": 4, \"unpack\": \"doubleword\" }", doc));
    JsonPixelStoreAlignment alignment(doc);
    ASSERT_TRUE(IsPixelStoreAlignment((const PixelStoreAlignment&) alignment, k4, kDoubleWordAlignment));
  }

  TEST_F(JsonTextureAlignmentTest, Test_UnpackIsRow) {
    json::Document doc;
    ASSERT_TRUE(json::ParseRawJson("{ \"unpack\": \"row\" }", doc));
    JsonPixelStoreAlignment alignment(doc);
    ASSERT_TRUE(IsPixelStoreAlignment((const PixelStoreAlignment&) alignment, k4, kRowAlignment));
  }
}