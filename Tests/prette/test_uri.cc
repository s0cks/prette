#include <set>
#include <gtest/gtest.h>
#include "prette/uri.h"
#include "prette/uri_parser.h"

namespace prt::uri {
  using namespace ::testing;
  class UriTest : public Test {
  protected:
    UriTest() = default;
  public:
    ~UriTest() override = default;
  };

  TEST_F(UriTest, Test_Parse_ExplicitProtocol) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("test://hello-world");
    ASSERT_EQ(uri.scheme, "test");
    ASSERT_EQ(uri.path, "hello-world");
  }

  TEST_F(UriTest, Test_HasScheme0) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("test://example");
    ASSERT_TRUE(uri.HasScheme());
  }

  TEST_F(UriTest, Test_HasScheme1) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("test://example");
    ASSERT_TRUE(uri.HasScheme("test"));
  }

  TEST_F(UriTest, Test_HasScheme2) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("test://example");
    ASSERT_TRUE(uri.HasScheme(std::vector<std::string>{ "test", "another" }));
  }

  TEST_F(UriTest, Test_HasExtension0) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("test:example.png");
    ASSERT_TRUE(uri.HasExtension());
  }

  TEST_F(UriTest, Test_HasExtension1) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("test:example.png");
    ASSERT_TRUE(uri.HasExtension("png"));
  }

  TEST_F(UriTest, Test_HasExtension2) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("test:example.png");
    ASSERT_TRUE(uri.HasExtension(std::vector<std::string>{ "png", "another" }));
  }

  TEST_F(UriTest, Test_GetQuery_Fails_NoKey) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("file://example.png");
    ASSERT_EQ(uri.scheme, "file");
    ASSERT_EQ(uri.path, "example.png");
    ASSERT_FALSE(uri.HasQuery());
    const auto q1 = uri.GetQuery("test");
    ASSERT_FALSE(q1);
  }

  TEST_F(UriTest, Test_Equality_WillFail_SchemeNotEqual) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri1("file://example.png");
    Uri uri2("test://example.png");
    ASSERT_NE(uri1, uri2);
  }

  TEST_F(UriTest, Test_Equality_WillFail_PathNotEqual) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri1("file://example.png");
    Uri uri2("file://example.jpeg");
    ASSERT_NE(uri1, uri2);
  }

  TEST_F(UriTest, Test_Equality_WillPass) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri1("file://example.png");
    Uri uri2("file://example.png");
    ASSERT_EQ(uri1, uri2);
  }

  TEST_F(UriTest, Test_StringCast) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "file://example.png";
    const auto uri = Uri(kRawUri);
    const auto str = ((const std::string&) uri);
    ASSERT_EQ(str, kRawUri);
  }

  TEST_F(UriTest, Test_GetParentPath) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("file:///root/example.png");
    ASSERT_EQ(uri.scheme, "file");
    ASSERT_EQ(uri.path, "/root/example.png");
    ASSERT_EQ(uri.GetParentPath(), "/root");
  }

  TEST_F(UriTest, Test_GetChildPath) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("file:///root");
    ASSERT_EQ(uri.scheme, "file");
    ASSERT_EQ(uri.path, "/root");
    ASSERT_EQ(uri.GetChildPath("test.png"), "/root/test.png");
  }

  TEST_F(UriTest, Test_GetSiblingPath) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Uri uri("file:///root");
    ASSERT_EQ(uri.scheme, "file");
    ASSERT_EQ(uri.path, "/root");
    ASSERT_EQ(uri.GetSiblingPath("test.png"), "/test.png");
  }
}