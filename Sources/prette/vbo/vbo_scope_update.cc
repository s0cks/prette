#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo.h"

namespace prt::vbo {
  uword VboUpdateScopeBase::GetElementSize() const {
    return GetVbo()->GetElementSize();
  }

  void VboUpdateScopeBase::Update(const uword offset, const Region& src) {
    PRT_ASSERT(offset >= 0);
    PRT_ASSERT(src.GetStartingAddress() != 0);
    PRT_ASSERT(!src.IsEmpty());
    if(GetVbo()->IsEmpty()) {
      PRT_ASSERT(offset == 0);
      Vbo::InitData(src, GetVbo()->GetUsage());
    } else {
      Vbo::PutData(offset, src);
    }
    GetVbo()->SetLength(src.GetSize() / GetElementSize());
  }
}