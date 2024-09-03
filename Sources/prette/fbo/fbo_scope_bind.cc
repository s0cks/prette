#include "prette/fbo/fbo_scope.h"
#include "prette/fbo/fbo.h"

namespace prt::fbo {
  void FboBindScope::Bind() const {
    Fbo::BindFbo(GetFboId());
  }

  void FboBindScope::Unbind() const {
    Fbo::BindDefaultFbo();
  }
}