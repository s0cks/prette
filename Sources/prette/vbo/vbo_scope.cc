#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo.h"

namespace prt::vbo {
  VboId VboScope::GetVboId() const {
    return GetVbo()->GetId();
  }

  WriteOnlyVboScope::WriteOnlyVboScope(Vbo* vbo):
    gfx::WriteOnlyMappedBufferScope<kGlTarget>(vbo->GetTotalSize()) {
  }

  ReadOnlyVboScope::ReadOnlyVboScope(Vbo* vbo):
    gfx::ReadOnlyMappedBufferScope<kGlTarget>(vbo->GetTotalSize()) {
  }
}