#include <gtest/gtest.h>

#include "prette/vao/vao_json.h"

namespace prt::vao {
  using namespace ::testing;

  class VaoJsonTest : public Test {
  protected:
    VaoJsonTest() = default;
  public:
    ~VaoJsonTest() override = default;
  };

  template<class Handler>
  static inline AssertionResult
  ParseIsSuccessful(const char* json, Handler& handler) {
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json);
    if(reader.Parse(ss, handler))
      return AssertionSuccess();
    const auto code = reader.GetParseErrorCode();
    const auto offset = reader.GetErrorOffset();
    return AssertionFailure() << "expected \"" << json << "\" to be valid json, received: \"" << rapidjson::GetParseError_En(code) << "\" at " << offset << " near: " << std::string(json, 10) << "....";
  }

  TEST_F(VaoJsonTest, Test_SAX) {
    static constexpr const auto kTestJson = 
      "{"
        "\"type\": \"Vao\","
        "\"meta\": {"
          "\"name\": \"test-vao\","
          "\"tags\": ["\
            "\"test\","
            "\"test-2\""
          "]"
        "}"
      "}";
    
    VaoReaderHandler handler;
    ASSERT_TRUE(ParseIsSuccessful(kTestJson, handler));
  }
}