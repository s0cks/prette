#include <gtest/gtest.h>

#include "prette/mock_gl.h"
#include "prette/gfx_blend_scope.h"

namespace prt::gfx {
  using namespace ::testing;

  class BlendTestScopeTest : public Test {
  protected:
    BlendTestScopeTest() = default;
  public:
    ~BlendTestScopeTest() override = default;
  };

  TEST_F(BlendTestScopeTest, Test_No_Restore) {
    MockGl gl;
    EXPECT_CALL(gl, glEnable(Eq(3042)));
    EXPECT_CALL(gl, glBlendFunc(Eq(kSrcAlpha), Eq(kOneMinusSrcAlpha)));
    EXPECT_CALL(gl, glBlendEquation(Eq(kAdd)));
    EXPECT_CALL(gl, glBlendFuncSeparate(_, _, _, _))
      .Times(0);
    EXPECT_CALL(gl, glDisable(Eq(3042)));
    EXPECT_CALL(gl, glGetError())
      .Times(AtLeast(1));
    {
      BlendTestScope<false> scope(kSrcAlpha, kOneMinusSrcAlpha, kAdd);
    }
  }

  TEST_F(BlendTestScopeTest, Test_With_Restore) {
    static constexpr const auto kSrcRgbValue = 0xFF0000;
    static constexpr const auto kSrcAlphaValue = 0xFF;

    static constexpr const auto kDstRgbValue = 0x0000FF;
    static constexpr const auto kDstAlphaValue = 0x00;
  
    MockGl gl;
    ON_CALL(gl, glGetIntegerv(Eq(GL_BLEND_SRC_RGB), _))
      .WillByDefault([](GLenum pname, GLint *params) {
        (*params) = kSrcRgbValue;
      });
    ON_CALL(gl, glGetIntegerv(Eq(GL_BLEND_SRC_ALPHA), _))
      .WillByDefault([](GLenum pname, GLint *params) {
        (*params) = kSrcAlphaValue;
      });
    ON_CALL(gl, glGetIntegerv(Eq(GL_BLEND_DST_RGB), _))
      .WillByDefault([](GLenum pname, GLint *params) {
        (*params) = kSrcAlphaValue;
      });
    ON_CALL(gl, glGetIntegerv(Eq(GL_BLEND_DST_ALPHA), _))
      .WillByDefault([](GLenum pname, GLint *params) {
        (*params) = kDstAlphaValue;
      });

    EXPECT_CALL(gl, glEnable(Eq(GL_BLEND)));
    EXPECT_CALL(gl, glBlendFunc(Eq(kSrcAlpha), Eq(kOneMinusSrcAlpha)));
    EXPECT_CALL(gl, glBlendEquation(Eq(kAdd)));
    EXPECT_CALL(gl, glGetIntegerv(Eq(GL_BLEND_SRC_RGB), _));
    EXPECT_CALL(gl, glGetIntegerv(Eq(GL_BLEND_SRC_ALPHA), _));
    EXPECT_CALL(gl, glGetIntegerv(Eq(GL_BLEND_DST_RGB), _));
    EXPECT_CALL(gl, glGetIntegerv(Eq(GL_BLEND_DST_ALPHA), _));
    EXPECT_CALL(gl, glBlendFuncSeparate(Eq(kSrcRgbValue), Eq(kSrcAlphaValue), Eq(kDstRgbValue), Eq(kDstAlphaValue)));
    EXPECT_CALL(gl, glDisable(Eq(GL_BLEND)));
    EXPECT_CALL(gl, glGetError())
      .Times(AtLeast(1));

    {
      BlendTestScope scope(kSrcAlpha, kOneMinusSrcAlpha, kAdd);
    }
  }

  class InvertedBlendTestScopeTest : public Test {
  protected:
    InvertedBlendTestScopeTest() = default;
  public:
    ~InvertedBlendTestScopeTest() override = default;
  };

  TEST_F(InvertedBlendTestScopeTest, Test) {

  }
}