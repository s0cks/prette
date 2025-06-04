#ifndef PRT_ENTITY_H
#define PRT_ENTITY_H

#include <string>

#include "prette/common.h"
#include "prette/entity/entity_event.h"
#include "prette/entity/entity_id.h"

namespace prt {
auto GetEntityEventObservable() -> EntityEventObservable;

template <typename... ArgN>
static inline auto OnEntityEvent(ArgN... args) -> rx::composite_subscription {
  return GetEntityEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                                                             \
  static inline auto Get##Name##EventObservable()->Name##EventObservable {                \
    return GetEntityEventObservable().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                                       \
  template <typename... ArgN>                                                             \
  static inline auto On##Name##Event(ArgN... args)->rx::composite_subscription {          \
    return Get##Name##EventObservable().subscribe(args...);                               \
  }
FOR_EACH_ENTITY_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

class Entity;
class EntityVisitor {
 public:
  EntityVisitor() = default;
  virtual ~EntityVisitor() = default;
  virtual auto Visit(Entity* entity) -> bool = 0;
};

class Entity {
  friend class EntitySystem;
  DEFINE_DEFAULT_COPYABLE_TYPE(Entity);

 private:
  static void PublishEvent(EntityEvent* rhs);

  template <typename E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

#define DEFINE_PUBLISH(Name)                              \
  template <typename... Args>                             \
  static inline void Publish##Name##Event(Args... args) { \
    return Publish<Name##Event>(args...);                 \
  }
  FOR_EACH_ENTITY_EVENT(DEFINE_PUBLISH)
#undef DEFINE_PUBLISH

 private:
  EntityId id_;

  explicit Entity(const EntityId id) :
    id_(id) {}

 public:
  Entity() = default;
  ~Entity() = default;

  auto GetId() const -> EntityId {
    return id_;
  }

  auto ToString() const -> std::string;

  operator EntityId() const {
    return id_;
  }

  auto operator==(const EntityId& rhs) const -> bool {
    return id_ == rhs;
  }

  auto operator==(const Entity& rhs) const -> bool {
    return operator==(rhs.id_);
  }

  auto operator!=(const EntityId& rhs) const -> bool {
    return id_ != rhs;
  }

  auto operator!=(const Entity& rhs) const -> bool {
    return operator!=(rhs.id_);
  }

  auto operator<(const EntityId& rhs) const -> bool {
    return id_ < rhs;
  }

  auto operator<(const Entity& rhs) const -> bool {
    return operator<(rhs.id_);
  }
};
}  // namespace prt

#endif  // PRT_ENTITY_H
