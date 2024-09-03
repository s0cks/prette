#include "prette/ibo/ibo_scope.h"

namespace prt::ibo {
  void IboBindScope::Bind() const {
    return Ibo::BindIbo(GetIboId());
  }

  void IboBindScope::Unbind() const {
    return Ibo::BindDefaultIbo();
  }
}