#include "prette/vao/vao_scope.h"
#include "prette/vao/vao.h"

namespace prt::vao {
  VaoId VaoScope::GetVaoId() const {
    return GetVao()->GetId();
  }
}