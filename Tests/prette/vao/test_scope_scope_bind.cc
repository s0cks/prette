#include <gtest/gtest.h>

#include "prette/mock_gl.h"
#include "prette/vao/mock_vao.h"
#include "prette/vao/vao_scope.h"

namespace prt::vao {
  using namespace ::testing;

  class VaoBindScopeTest : public Test {
  protected:
    VaoBindScopeTest() = default;
  public:
    ~VaoBindScopeTest() override = default;
  };

  TEST_F(VaoBindScopeTest, Test) {
    static constexpr const auto kVaoId = 102;

    MockVao vao(kVaoId);

    MockGl gl;
    EXPECT_CALL(gl, glBindVertexArray(Eq(kVaoId)));
    EXPECT_CALL(gl, glBindVertexArray(Eq(0)));

    {
      VaoBindScope scope(&vao);
    }
  }
}