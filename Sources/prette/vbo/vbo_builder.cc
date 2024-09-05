#include "prette/vbo/vbo_builder.h"
#include "prette/vbo/vbo_attr_binder.h"

namespace prt::vbo {
  Vbo* VboBuilderBase::Build() const {
    const auto source = GetSource();
    PRT_ASSERT(source.GetStartingAddress() >= 0);
    PRT_ASSERT(!source.IsEmpty());

    const auto id = VboId::GenerateId();
    PRT_ASSERT(id.IsValid());
    Vbo::Bind(id);
    VboAttributeBinder::BindAll(GetClass());
    Vbo::InitData(source, GetUsage());
    Vbo::Unbind();
    return Vbo::New(id, GetClass(), GetLength(), GetUsage());
  }
}