#include <gtest/gtest.h>

#include "prette/mock_gl.h"
#include "prette/vao/vao.h"

namespace prt::vao {
  using namespace ::testing;

  class VaoTest : public Test {
  protected:
    VaoTest() = default;

    static inline void BindVao(const VaoId id) {
      return Vao::BindVao(id);
    }

    static inline void DeleteVaos(VaoId* ids, const int num_ids) {
      return Vao::DeleteVaos(ids, num_ids);
    }
  public:
    ~VaoTest() override = default;
  };

  TEST_F(VaoTest, Test_BindVao_WillPass_IdIsZero) {
    static constexpr const VaoId kVaoId = 0;
    MockGl gl;
    EXPECT_CALL(gl, glBindVertexArray(Eq(kVaoId)));
    BindVao(kVaoId);
  }

  TEST_F(VaoTest, Test_BindVao_WillPass_IsIsValid) {
    static constexpr const VaoId kVaoId = 10;
    MockGl gl;
    EXPECT_CALL(gl, glBindVertexArray(Eq(kVaoId)));
    BindVao(kVaoId);
  }

  TEST_F(VaoTest, Test_DeleteVaos_WillFail_NumIdsLessThanZero) {
    MockGl gl;
    ASSERT_DEATH(DeleteVaos(nullptr, -1), "");
  }

  TEST_F(VaoTest, Test_DeleteVaos_WillFail_NumIdsZero) {
    MockGl gl;
    ASSERT_DEATH(DeleteVaos(nullptr, 0), "");
  }
}