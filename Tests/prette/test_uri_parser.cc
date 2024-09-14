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
  IsSuccess(uri::Parser& parser) -> AssertionResult {
    return IsSuccess(parser.Parse());
  }

  static inline auto
  IsFailure(const uri::ParseResult& result) -> AssertionResult {
    if(!result)
      return AssertionSuccess() << result << " is not successful.";
    return AssertionFailure() << "expected " << result << " to not be successful.";
  }

  static inline auto
  IsFailure(Parser& parser) -> AssertionResult {
    return IsFailure(parser.Parse());
  }

  TEST_F(UriParserTest, Test_ParseFails_InvalidScheme1) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser("tex:/test.png");
    ASSERT_TRUE(IsFailure(parser));
  }

  TEST_F(UriParserTest, Test_Parse_DefaultScheme) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    MockUriParserConfig config;
    EXPECT_CALL(config, OnParseScheme(Eq("default")));
    Parser parser(Parser::Config {
      .default_scheme = "default",
      .OnParseScheme = &MockUriParserConfig::OnParseSchemeWrapper,
    }, "test.png", &config);
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_ParseFails_InvalidScheme) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser(":test.png");
    ASSERT_TRUE(IsFailure(parser));
  }

  TEST_F(UriParserTest, Test_Parse0) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser("t:test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse1) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser("t://test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse2) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser("tex://test.png");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse3) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse4) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse5) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?message=hello");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse6) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?message=hello&");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse7) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?message=hello&test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse8) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries() | Parser::ParseFragments(),
    }, "tex://test.png?message=hello&test#test");
    ASSERT_TRUE(IsSuccess(parser));
  }
}