#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo.h"

namespace prt::vbo {
  VboId VboScope::GetVboId() const {
    return GetVbo()->GetId();
  }

  WriteOnlyVboScope::WriteOnlyVboScope(Vbo* vbo):
    gfx::WriteOnlyMappedBufferScope<Vbo::kGlTarget>(vbo->GetTotalSize()) { //TODO: use Vbo::kGlType
  }

  ReadOnlyVboScope::ReadOnlyVboScope(Vbo* vbo):
    gfx::ReadOnlyMappedBufferScope<Vbo::kGlTarget>(vbo->GetTotalSize()) { //TODO: use Vbo::kGlType
  }
}