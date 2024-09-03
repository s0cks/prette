#include "prette/vao/vao_scope.h"
#include "prette/vao/vao.h"

namespace prt::vao {
  void VaoBindScope::Bind() const {
    Vao::BindVao(GetVaoId());
  }

  void VaoBindScope::Unbind() const {
    Vao::UnbindVao();
  }
}