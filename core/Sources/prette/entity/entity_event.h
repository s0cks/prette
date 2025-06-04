#ifndef PRT_ENTITY_EVENT_H
#define PRT_ENTITY_EVENT_H

#include "prette/entity/entity_id.h"
#include "prette/event.h"

namespace prt {
#define FOR_EACH_ENTITY_EVENT(V) \
  V(EntityCreated)               \
  V(EntitySignatureChanged)      \
  V(EntityDestroyed)

class EntityEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_ENTITY_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class EntityEvent : public Event {
 private:
  EntityId id_;

 protected:
  explicit EntityEvent(const EntityId id) :
    Event(),
    id_(id) {}

 public:
  ~EntityEvent() override = default;

  auto GetEntityId() const -> EntityId {
    return id_;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(Entity, FOR_EACH_ENTITY_EVENT);
};

#define DECLARE_ENTITY_EVENT(Name)            \
  class Name##Event : public EntityEvent {    \
   public:                                    \
    explicit Name##Event(const EntityId id) : \
      EntityEvent(id) {}                      \
    ~Name##Event() override = default;        \
    DECLARE_EVENT_TYPE(EntityEvent, Name);    \
  };
FOR_EACH_ENTITY_EVENT(DECLARE_ENTITY_EVENT)
#undef DECLARE_ENTITY_EVENT

DEFINE_EVENT_OBSERVABLE(Entity);
DEFINE_EVENT_SUBJECT(Entity);
FOR_EACH_ENTITY_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_ENTITY_EVENT_H
