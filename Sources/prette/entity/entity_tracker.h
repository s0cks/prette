#ifndef PRT_ENTITY_TRACKER_H
#define PRT_ENTITY_TRACKER_H

#include "prette/entity/entity_id.h"
#include "prette/entity/entity_set.h"
#include "prette/entity/entity_events.h"
#include "prette/entity/entity_signature.h"

namespace prt::entity {
  class EntityVisitor;
  class EntityTracker {
  private:
    rx::subscription on_created_;
    rx::subscription on_destroyed_;
    EntitySet entities_;
  protected:
    virtual void OnCreated(const EntityId id) {
      entities_.Put(id);
    }

    virtual void OnDestroyed(const EntityId id) {
      entities_.Remove(id);
    }

    inline auto entities() -> EntitySet& {
      return entities_;
    }

    inline auto entities() const -> const EntitySet& {
      return entities_;
    }
  public:
    EntityTracker();

    auto GetEntities() const -> const EntitySet& {
      return entities_;
    }

    virtual ~EntityTracker() {
      on_created_.unsubscribe();
      on_destroyed_.unsubscribe();
    }

    virtual auto Contains(const EntityId id) const -> bool {
      return entities_.Contains(id);
    }

    virtual auto VisitAll(std::function<bool(EntityId)> vis) const -> bool;

    explicit operator rx::observable<EntityId> () const {
      return (rx::observable<EntityId>) entities_;
    }
  };

  class FilteredEntityTracker : public EntityTracker {
  private:
    Signature signature_;
    rx::subscription on_signature_changed_;
  protected:
    void OnCreated(const EntityId id) override { }

    inline auto
    HasSignature(const Signature& sig) -> bool {
      return (sig & signature_) == signature_;
    }

    virtual void OnSignatureChanged(const EntityId id, const Signature& new_sig) {
      if(HasSignature(new_sig)) {
        entities().Put(id);
        return;
      }
      entities().Remove(id);
      return;
    }

    void OnDestroyed(const EntityId id) override { }
  public:
    FilteredEntityTracker();
    explicit FilteredEntityTracker(const Signature& signature):
      FilteredEntityTracker() {
      SetSignature(signature);
    }
    ~FilteredEntityTracker() override {
      on_signature_changed_.unsubscribe();
    }

    auto GetSignature() const -> const Signature& {
      return signature_;
    }

    void SetSignature(const Signature& signature);

    explicit operator rx::observable<EntityId> () const {
      return (rx::observable<EntityId>) entities();
    }
  };
}

#endif //PRT_ENTITY_TRACKER_H