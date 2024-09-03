#ifndef PRT_MATERIAL_LOADER_H
#define PRT_MATERIAL_LOADER_H

#include "prette/rx.h"
#include "prette/material/material_component.h"

namespace prt::material {
  class Material;
  class MaterialLoader {
  protected:
    MaterialLoader() = default;
    virtual std::string GetMaterialName() const = 0;
    virtual rx::observable<MaterialComponent> GetMaterialComponents() const = 0;
  public:
    virtual ~MaterialLoader() = default;
    virtual Material* LoadMaterial() const;
  };
}

#endif //PRT_MATERIAL_LOADER_H