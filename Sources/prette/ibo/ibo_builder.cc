#include "prette/ibo/ibo_builder.h"

namespace prt::ibo {
  Ibo* IboBuilderBase::Build() const {
    const auto id = GenerateIboId();
    if(IsInvalidIboId(id)) {
      LOG(ERROR) << "failed to generate valid Ibo id: " << id;
      return nullptr;
    }

    const auto source = GetSource();
    Ibo::BindIbo(id);
    if(!source.IsEmpty())
      Ibo::InitBufferData(source, GetUsage());
    Ibo::BindDefaultIbo();
    return new Ibo(id, GetClass(), GetLength(), GetUsage());
  }
}