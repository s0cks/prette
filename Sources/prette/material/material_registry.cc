#include "prette/material/material_registry.h"
#include "prette/material/material.h"

namespace prt::material {
  bool MaterialRegistry::Comparator::operator()(const Material* lhs, const Material* rhs) const {
    return false;
  }

  MaterialRegistry::MaterialRegistry(const rx::observable<MaterialEvent*>& events):
    on_created_(),
    on_destroyed_() {
    on_created_ = events
      .filter(MaterialCreatedEvent::Filter)
      .map(MaterialCreatedEvent::Cast)
      .subscribe([this](MaterialCreatedEvent* event) {
        return Register(event->GetMaterial());
      });
    on_destroyed_ = events
      .filter(MaterialDestroyedEvent::Filter)
      .map(MaterialDestroyedEvent::Cast)
      .subscribe([this](MaterialDestroyedEvent* event) {
        return Deregister(event->GetMaterial());
      });
  }

  MaterialRegistry::~MaterialRegistry() {
    if(on_created_.is_subscribed())
      on_created_.unsubscribe();
    if(on_destroyed_.is_subscribed())
      on_destroyed_.unsubscribe();
  }

  void MaterialRegistry::Register(const Material* material) {
    const auto result = materials_.insert(material);
    LOG_IF(ERROR, !result.second) << "failed to register: " << material->ToString();
  }

  void MaterialRegistry::Deregister(const Material* material) {
    const auto result = materials_.erase(material);
    LOG_IF(ERROR, result != 1) << "failed to deregister: " << material->ToString();
  }
}