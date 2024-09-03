#include <gtest/gtest.h>

#include "prette/shader/shader_code.h"
#include "prette/shader/shader_code_assertions.h"

namespace prt::shader {
  using namespace ::testing;

  class ShaderCodeTest : public Test {
  protected:
    ShaderCodeTest() = default;
  public:
    ~ShaderCodeTest() override = default;
  };

  TEST_F(ShaderCodeTest, Test_New_WillFail_NullData) {
    const auto code = ShaderCode::New(ShaderType::kVertexShader, (const RawShaderCode) nullptr, 0);
    ASSERT_FALSE(code);
  }

  TEST_F(ShaderCodeTest, Test_New_WillFail_LengthZero) {
    uint8_t data = 0x11;
    const auto code = ShaderCode::New(ShaderType::kVertexShader, &data, 0);
    ASSERT_FALSE(code);
  }

  TEST_F(ShaderCodeTest, Test_New_WillPass) {
    static constexpr const auto kShaderCode = "Hello World";
    const auto code = ShaderCode::NewVertexShaderCode(kShaderCode);
    ASSERT_TRUE(code);
    DLOG(INFO) << "code: " << code->ToString();
    ASSERT_TRUE(VertexShaderCodeEquals(code, kShaderCode));
  }

  TEST_F(ShaderCodeTest, Test_VertexShaderCodeFromFile_WillPass) {
    static constexpr const auto kExpectedShaderCode = 
      "#version 330 core\n"
      "void main() {\n"
      "}";
    const auto code = ShaderCode::VertexShaderCodeFromFile("shader:test-shader-code-1.glsl");
    ASSERT_TRUE(code);
    DLOG(INFO) << "code: " << code->ToString();
    ASSERT_TRUE(VertexShaderCodeEquals(code, kExpectedShaderCode));
  }

  TEST_F(ShaderCodeTest, Test_FromFile_WillPass) {
    static constexpr const auto kExpectedShaderCode = 
      "#version 330 core\n"
      "void main() {\n"
      "}";
    const auto code = ShaderCode::FromFile("shader:test-shader-code-1.vert");
    ASSERT_TRUE(code);
    DLOG(INFO) << "code: " << code->ToString();
    ASSERT_TRUE(VertexShaderCodeEquals(code, kExpectedShaderCode));
  }
}