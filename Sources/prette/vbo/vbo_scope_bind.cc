#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo.h"

namespace prt::vbo {
  void VboBindScope::Bind() const {
    Vbo::Bind(GetVboId());
  }

  void VboBindScope::Unbind() const {
    Vbo::BindDefaultVbo();
  }
}