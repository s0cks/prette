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

  static inline AssertionResult
  IsSuccess(const uri::ParseResult& result) {
    if(!result)
      return AssertionFailure() << "expected " << result << " to be successful.";
    return AssertionSuccess() << result << " is successful.";
  }

  static inline AssertionResult
  IsSuccess(uri::Parser& parser) {
    return IsSuccess(parser.Parse());
  }

  static inline AssertionResult
  IsFailure(const uri::ParseResult& result) {
    if(!result)
      return AssertionSuccess() << result << " is not successful.";
    return AssertionFailure() << "expected " << result << " to not be successful.";
  }

  static inline AssertionResult
  IsFailure(Parser& parser) {
    return IsFailure(parser.Parse());
  }

  TEST_F(UriParserTest, Test_ParseFails_InvalidScheme1) {
    Parser parser("tex:/test.png");
    ASSERT_TRUE(IsFailure(parser));
  }

  TEST_F(UriParserTest, Test_Parse_DefaultScheme) {
    MockUriParserConfig config;
    EXPECT_CALL(config, OnParseScheme(Eq("default")));
    Parser parser(Parser::Config {
      .default_scheme = "default",
      .OnParseScheme = &MockUriParserConfig::OnParseSchemeWrapper,
    }, "test.png", &config);
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_ParseFails_InvalidScheme) {
    Parser parser(":test.png");
    ASSERT_TRUE(IsFailure(parser));
  }

  TEST_F(UriParserTest, Test_Parse0) {
    Parser parser("t:test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse1) {
    Parser parser("t://test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse2) {
    Parser parser("tex://test.png");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse3) {
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse4) {
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse5) {
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?message=hello");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse6) {
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?message=hello&");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse7) {
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries(),
    }, "tex://test.png?message=hello&test");
    ASSERT_TRUE(IsSuccess(parser));
  }

  TEST_F(UriParserTest, Test_Parse8) {
    Parser parser(Parser::Config {
      .flags = Parser::Strict() | Parser::ParseQueries() | Parser::ParseFragments(),
    }, "tex://test.png?message=hello&test#test");
    ASSERT_TRUE(IsSuccess(parser));
  }
}