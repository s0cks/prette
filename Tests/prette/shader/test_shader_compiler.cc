// #include <gtest/gtest.h>
// #include <gmock/gmock.h>

// #include "prette/mock_gl.h"
// #include "prette/shader/shader_compiler.h"

// namespace prt::shader {
//   class ShaderCompilerTest : public testing::Test {
//   protected:
//     ShaderCompilerTest() = default;
//   public:
//     ~ShaderCompilerTest() override = default;
//   };

//   static inline ::testing::AssertionResult
//   IsInvalidShader(const ShaderId id) {
//     return id == kInvalidShaderId
//           ? ::testing::AssertionSuccess()
//           : ::testing::AssertionFailure() << "expected " << id << " to be an invalid shader id (" << kInvalidShaderId << ").";
//   }

//   TEST_F(ShaderCompilerTest, Test_Compile_WillFail_SourceEmpty) {
//     const auto code = ShaderCode::New(ShaderType::kFragmentShader, "");
//     MockGl gl;
//     ShaderCompiler compiler;
//     const auto id = compiler.Compile(code);
//     ASSERT_TRUE(IsInvalidShader(id));
//   }

//   TEST_F(ShaderCompilerTest, Test_Compile_WillFail_glCreateShaderReturnsInvalidShaderId) {
//     const auto code = ShaderCode::New(ShaderType::kFragmentShader, "this is a test.");
//     MockGl gl;
//     ON_CALL(gl, glCreateShader(::testing::_))
//       .WillByDefault([]() {
//         return kInvalidShaderId;
//       });

//     ShaderCompiler compiler;
//     const auto id = compiler.Compile(code);
//     ASSERT_TRUE(IsInvalidShader(id));
//   }

//   TEST_F(ShaderCompilerTest, Test_Compile_WillFail_glCompileFails) {
//     static constexpr const ShaderId kTestShaderId = 1;
//     static const std::string kTestShaderCode = "this is a test.";
//     static const std::string kTestErrorMessage = "error.";

//     const auto code = ShaderCode::New(ShaderType::kFragmentShader, "this is a test.");
//     MockGl gl;
//     ON_CALL(gl, glCreateShader(::testing::_))
//       .WillByDefault([]() {
//         return kTestShaderId;
//       });
//     ON_CALL(gl, glGetShaderiv(::testing::Eq(kTestShaderId), ::testing::Eq(GL_COMPILE_STATUS), ::testing::_))
//       .WillByDefault([](GLuint shader, GLenum pname, GLint *params) {
//         (*params) = 0;
//       });
//     ON_CALL(gl, glGetShaderInfoLog(::testing::Eq(kTestShaderId), ::testing::_, ::testing::_, ::testing::_))
//       .WillByDefault([](GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
//         (*length) = kTestErrorMessage.length();
//         memset(&infoLog[0], '\0', bufSize);
//         memcpy(&infoLog[0], kTestErrorMessage.data(), kTestErrorMessage.length());
//       });
//     EXPECT_CALL(gl, glShaderSource(::testing::Eq(kTestShaderId), ::testing::Eq(1), ::testing::_, ::testing::_));
//     EXPECT_CALL(gl, glCompileShader(::testing::Eq(kTestShaderId)));

//     ShaderCompiler compiler;
//     const auto id = compiler.Compile(code);
//     ASSERT_EQ(id, kInvalidShaderId);
//   }

//   TEST_F(ShaderCompilerTest, Test_Compile_WillPass) {
//     static constexpr const ShaderId kTestShaderId = 1;
//     static const std::string kTestShaderCode = "this is a test.";
//     static const std::string kTestErrorMessage = "error.";

//     const auto code = ShaderCode::New(ShaderType::kFragmentShader, "this is a test.");
//     MockGl gl;
//     ON_CALL(gl, glCreateShader(::testing::_))
//       .WillByDefault([]() {
//         return kTestShaderId;
//       });
//     ON_CALL(gl, glGetShaderiv(::testing::Eq(kTestShaderId), ::testing::Eq(GL_COMPILE_STATUS), ::testing::_))
//       .WillByDefault([](GLuint shader, GLenum pname, GLint *params) {
//         (*params) = 1;
//       });
//     ON_CALL(gl, glGetShaderInfoLog(::testing::Eq(kTestShaderId), ::testing::_, ::testing::_, ::testing::_))
//       .WillByDefault([](GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
//         memset(&infoLog[0], '\0', bufSize);
//       });
//     EXPECT_CALL(gl, glShaderSource(::testing::Eq(kTestShaderId), ::testing::Eq(1), ::testing::_, ::testing::_));
//     EXPECT_CALL(gl, glCompileShader(::testing::Eq(kTestShaderId)));

//     ShaderCompiler compiler;
//     const auto id = compiler.Compile(code);
//     ASSERT_EQ(id, kTestShaderId);
//   }
// }