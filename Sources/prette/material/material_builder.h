#ifndef PRT_MATERIAL_BUILDER_H
#define PRT_MATERIAL_BUILDER_H

#include <string>
#include "prette/rx.h"
#include "prette/material/material_component.h"

namespace prt::material {
  class Material;
  class MaterialBuilder {
  protected:
    std::string name_;
    MaterialComponentSet components_;
  public:
    explicit MaterialBuilder(const std::string& name,
                             const MaterialComponentSet& components = {}):
      name_(name),
      components_(components) {
    }
    ~MaterialBuilder() = default;

    const std::string& GetName() const {
      return name_;
    }

    const MaterialComponentSet& GetComponents() const {
      return components_;
    }

    void Append(const MaterialComponent& rhs);
    void Append(const MaterialComponentSet& rhs);
    void Append(const MaterialComponentList& rhs);

    virtual Material* Build() const;

    virtual rx::observable<Material*> BuildAsync() const {
      return rx::observable<>::create<Material*>([this](rx::subscriber<Material*> s) {
        const auto material = Build();
        if(!material)
          return s.on_error(rx::util::make_error_ptr(std::runtime_error("Failed to build new Material")));
        s.on_next(material);
        s.on_completed();
      });
    }
  };
}

#endif //PRT_MATERIAL_BUILDER_H