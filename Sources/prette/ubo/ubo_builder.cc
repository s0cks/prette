#include "prette/ubo/ubo_builder.h"

#include <fmt/format.h>
#include "prette/ubo/ubo.h"

namespace prt::ubo {
  UboBuilderScope::UboBuilderScope(const UboId id):
    id_(id) {
    PRT_ASSERT(IsValidUboId(id));
    Ubo::BindUbo(id);
  }

  UboBuilderScope::~UboBuilderScope()  {
    Ubo::UnbindUbo();
  }

  void UboBuilderBase::Init(const UboId id) const {
    PRT_ASSERT(IsValidUboId(id));
    UboBuilderScope scope(id);
    Ubo::InitUbo(GetData(), GetTotalSize(), GetUsage());
  }

  Ubo* UboBuilderBase::Build() const {
    const auto id = GenerateUboId();
    Init(id);
    return new Ubo(id, GetElementSize(), GetLength(), GetUsage());
  }

  rx::observable<Ubo*> UboBuilderBase::BuildAsync() const {
    return rx::observable<>::create<Ubo*>([this](rx::subscriber<Ubo*> s) {
      const auto ubo = Build();
      if(!ubo) {
        const auto err = fmt::format("failed to build Ubo.");
        return s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
      }

      s.on_next(ubo);
      s.on_completed();
    });
  }
}