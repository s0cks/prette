#include "prette/ubo/ubo_scope.h"
#include "prette/ubo/ubo.h"

namespace prt::ubo {
  void UboBindScope::Bind() const {
    Ubo::BindUbo(GetUboId());
  }

  void UboBindScope::Unbind() const {
    Ubo::UnbindUbo();
  }
}