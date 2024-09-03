#ifndef PRT_MOCK_VAO_H
#define PRT_MOCK_VAO_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "prette/vao/vao.h"

namespace prt::vao {
  class MockVao : public Vao {
  public:
    explicit MockVao(const VaoId id, const Metadata& meta = {}):
      Vao(id, meta) { //TODO: use mock metadata
    }
    ~MockVao() override = default;
  };
}

#endif //PRT_MOCK_VAO_H