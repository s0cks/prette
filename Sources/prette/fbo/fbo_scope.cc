#include "prette/fbo/fbo_scope.h"
#include "prette/fbo/fbo.h"

namespace prt::fbo {
  FboId FboScope::GetFboId() const {
    return GetFbo()->GetId();
  }
}