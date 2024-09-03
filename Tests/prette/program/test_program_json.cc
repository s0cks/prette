#include <gtest/gtest.h>

#include "prette/program/program_json.h"

namespace prt::program {
  using namespace ::testing;

  class ProgramJsonTest : public Test {
  protected:
    ProgramJsonTest() = default;
  public:
    ~ProgramJsonTest() override = default;
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
    return AssertionFailure() << "expected \"" << json << "\" to be valid json, received: \"" << rapidjson::GetParseError_En(code) << "\" at " << offset << " near: " << std::string(&json[offset], 10) << "....";
  }

  TEST_F(ProgramJsonTest, Test_SAX) {
    static constexpr const auto kTestJson = 
      "{"
        "\"type\": \"Program\","
        "\"meta\": {"
          "\"name\": \"test-program\","
          "\"tags\": ["
            "\"test\""
          "]"
        "},"
        "\"data\": {"
          "\"vertex\": \"test.vert\","
          "\"fragment\": \"test.frag\""
        "}"
      "}";
    
    ProgramReaderHandler handler;
    ASSERT_TRUE(ParseIsSuccessful(kTestJson, handler));

    const auto& meta = handler.GetMeta();
    DLOG(INFO) << "meta: " << meta;
    ASSERT_EQ(meta.GetName(), "test-program");
    
    const auto& tags = meta.GetTags();
    {
      DLOG(INFO) << "tags: ";
      for(const auto& tag : tags)
        DLOG(INFO) << " - " << tag;
    }
    ASSERT_EQ(tags.size(), 1);

    DLOG(INFO) << "shaders:";
    const auto& shaders = handler.shaders();
    {
      for(const auto& shader : shaders)
        DLOG(INFO) << " - " << shader;
    }
    ASSERT_GE(shaders.size(), 2);
  }
}