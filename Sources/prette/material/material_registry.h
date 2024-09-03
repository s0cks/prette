#ifndef PRT_MATERIAL_REGISTRY_H
#define PRT_MATERIAL_REGISTRY_H

#include <set>
#include "prette/material/material_events.h"

namespace prt::material {
  class MaterialRegistry {
  public:
    struct Comparator {
      bool operator()(const Material* lhs, const Material* rhs) const;
    };

    typedef std::set<const Material*, Comparator> MaterialSet;
  protected:
    MaterialSet materials_;
    rx::subscription on_created_;
    rx::subscription on_destroyed_;

    explicit MaterialRegistry(const rx::observable<MaterialEvent*>& events);
    explicit MaterialRegistry(const rx::subject<MaterialEvent*>& events):
      MaterialRegistry(events.get_observable()) {
    }

    void Register(const Material* material);
    void Deregister(const Material* material);
  public:
    virtual ~MaterialRegistry();
  };
}

#endif //PRT_MATERIAL_REGISTRY_H