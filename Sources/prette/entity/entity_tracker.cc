#include "prette/entity/entity_tracker.h"

#include "prette/entity/entity.h"

namespace prt::entity {
  EntityTracker::EntityTracker():
    on_created_(),
    on_destroyed_(),
    entities_() {
    on_created_ = entity::OnEntityCreatedEvent()
      .subscribe([this](entity::EntityCreatedEvent* event) {
        return OnCreated(event->id());
      });
    on_destroyed_ = entity::OnEntityDestroyedEvent()
      .subscribe([this](entity::EntityDestroyedEvent* event) {
        return OnDestroyed(event->id());
      });
  }

  bool EntityTracker::VisitAll(std::function<bool(EntityId)> vis) const {
    for(const auto& e : entities_) {
      if(!vis(e))
        return false;
    }
    return true;
  }

  FilteredEntityTracker::FilteredEntityTracker():
    EntityTracker(),
    signature_(),
    on_signature_changed_() {
  }

  void FilteredEntityTracker::SetSignature(const Signature& signature) {
    if(on_signature_changed_.is_subscribed())
      on_signature_changed_.unsubscribe();
      
    signature_ = signature;
    on_signature_changed_ = entity::OnEntitySignatureChangedEvent()
      .subscribe([this](entity::EntitySignatureChangedEvent* event) {
        return OnSignatureChanged(event->id(), event->signature());
      });
  }
}