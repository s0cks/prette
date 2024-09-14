#include <gtest/gtest.h>
#include "prette/uri_parser.h"
#include "prette/mock_uri_parser_config.h"

namespace prt::uri {
  using namespace ::testing;

  class UriParserTest : public Test {
  protected:
    UriParserTest() = default;
  public:
    ~UriParserTest() override = default;
  };

  static inline auto
  IsSuccess(const uri::ParseResult& result) -> AssertionResult {
    if(!result)
      return AssertionFailure() << "expected " << result << " to be successful.";
    return AssertionSuccess() << result << " is successful.";
  }

  static inline auto
  IsSuccess(uri::Parser& parser, const uri::basic_uri& uri) -> AssertionResult {
    return IsSuccess(parser.Parse(uri));
  }

  static inline auto
  IsFailure(const uri::ParseResult& result) -> AssertionResult {
    if(!result)
      return AssertionSuccess() << result << " is not successful.";
    return AssertionFailure() << "expected " << result << " to not be successful.";
  }

  static inline auto
  IsFailure(Parser& parser, const uri::basic_uri& uri) -> AssertionResult {
    return IsFailure(parser.Parse(uri));
  }

  TEST_F(UriParserTest, Test_Parse_DefaultScheme) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "test.png";
    MockUriParserConfig config;
    EXPECT_CALL(config, OnParseScheme(Eq("default")));
    Parser parser(Parser::Config {
      .default_scheme = "default",
      .OnParseScheme = &MockUriParserConfig::OnParseSchemeWrapper,
    }, &config);
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse_Fails_InvalidScheme) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = ":test.png";
    Parser parser;
    ASSERT_TRUE(IsFailure(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse0) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "t:test";
    Parser parser;
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse1) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "t://test";
    Parser parser;
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse2) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "tex://test.png";
    Parser parser;
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse3) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "tex://test.png?";
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    });
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse4) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "tex://test.png?test";
    MockUriParserConfig config;
    EXPECT_CALL(config, OnParseScheme(Eq("tex")));
    EXPECT_CALL(config, OnParseQuery0(_, StrEq("test"), _));
    Parser parser(Parser::Config {
      .flags = Parser::ParseQueries(),
      .OnParseScheme = &MockUriParserConfig::OnParseSchemeWrapper,
      .OnParseQuery0 = &MockUriParserConfig::OnParseQuery0Wrapper,
    }, &config);
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse5) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "tex://test.png?message=hello";
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    });
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse6) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "tex://test.png?message=hello&";
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    });
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse7) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "tex://test.png?message=hello&test";
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    });
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }

  TEST_F(UriParserTest, Test_Parse8) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static constexpr const auto kRawUri = "tex://test.png?message=hello&test#test";
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries() | Parser::ParseFragments(),
    });
    ASSERT_TRUE(IsSuccess(parser, kRawUri));
  }
}