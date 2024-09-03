#include "prette/shader/shader_unit_builder.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  ShaderUnit* ShaderUnitBuilder::Build() const {
    return ShaderUnit::New(GetMeta(), GetType(), GetCode());
  }

  rx::observable<ShaderUnit*> ShaderUnitBuilder::BuildAsync() const {
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