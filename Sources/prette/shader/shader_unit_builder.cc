#include "prette/shader/shader_unit_builder.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  auto ShaderUnitBuilder::Build() const -> ShaderUnit* {
    return ShaderUnit::New(GetMeta(), GetType(), GetCode());
  }

  auto ShaderUnitBuilder::BuildAsync() const -> rx::observable<ShaderUnit*> {
    return rx::observable<>::create<ShaderUnit*>([this](rx::subscriber<ShaderUnit*> s) {
      const auto unit = Build();
      if(!unit) {
        const auto err = "failed to build ShaderUnit.";
        return s.on_error(rx::util::make_error_ptr(err));
      }

      s.on_next(unit);
      s.on_completed();
    });
  }
}