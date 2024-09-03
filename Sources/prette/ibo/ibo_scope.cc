#include "prette/ibo/ibo_scope.h"

namespace prt::ibo {
  IboId IboScope::GetIboId() const {
    return GetIbo()->GetId();
  }

  ReadOnlyIboScope::ReadOnlyIboScope(Ibo* ibo):
    gfx::ReadOnlyMappedBufferScope<GL_ELEMENT_ARRAY_BUFFER>(ibo->GetTotalSize()) { //TODO: use Ibo::kGlType
  }

  WriteOnlyIboScope::WriteOnlyIboScope(Ibo* ibo):
    gfx::WriteOnlyMappedBufferScope<GL_ELEMENT_ARRAY_BUFFER>(ibo->GetTotalSize()) { //TODO: use Ibo::kGlType
  }
}