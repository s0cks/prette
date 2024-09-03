#include <glog/logging.h>

#include "prette/flags.h"
#include "prette/test_rapidjson.h"

namespace prt {
  static constexpr const char* kMessage = "Hello World";
  static constexpr const char* kJson = "{\"message\":\"Hello World\"}";

  TEST_F(RapidJsonTest, Test_ToJson) {
    json::StringBuffer buffer;
    json::Writer<json::StringBuffer> writer(buffer);
    
    writer.StartObject();
    {
      writer.Key("message");
      writer.String(kMessage);
    }
    writer.EndObject();

    const auto doc = std::string(buffer.GetString(), buffer.GetSize());
    ASSERT_EQ(doc, kJson);
  }

  TEST_F(RapidJsonTest, Test_FromJson) {
    json::StringStream ss(kJson);
    json::Document doc;
    doc.ParseStream(ss);
    ASSERT_TRUE(doc["message"].IsString());
    ASSERT_TRUE(strncmp(doc["message"].GetString(), kMessage, strlen(kMessage)) == 0);
  }

  TEST_F(RapidJsonTest, Test_SchemaValidation_WillFail) {
    ASSERT_FALSE(json::Validate(FLAGS_resources + "/test-invalid.json", FLAGS_resources + "/test.schema.json"));
  }

  TEST_F(RapidJsonTest, Test_SchemaValidation_WillPass) {
    ASSERT_TRUE(json::Validate(FLAGS_resources + "/test-valid.json", FLAGS_resources + "/test.schema.json"));
  }
}