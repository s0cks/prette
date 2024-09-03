#include "prette/material/material_builder.h"
#include "prette/material/material.h"

namespace prt::material {
  void MaterialBuilder::Append(const MaterialComponent& rhs) {
    const auto result = components_.insert(rhs);
    LOG_IF(ERROR, !result.second) << "failed to append component: " << rhs;
  }

  void MaterialBuilder::Append(const MaterialComponentSet& rhs) {
    components_.insert(std::begin(rhs), std::end(rhs));
  }

  void MaterialBuilder::Append(const MaterialComponentList& rhs) {
    components_.insert(std::begin(rhs), std::end(rhs));
  }

  Material* MaterialBuilder::Build() const {
    return new Material(name_, components_);
  }
}