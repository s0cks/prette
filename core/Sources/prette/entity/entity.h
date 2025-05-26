#ifndef PRT_ENTITY_H
#define PRT_ENTITY_H

#include <cstdint>
#include <string>

#include "prette/common.h"

namespace prt {
using EntityId = uint64_t;

class Entity;
class EntityVisitor {
 public:
  EntityVisitor() = default;
  virtual ~EntityVisitor() = default;
  virtual auto Visit(Entity* entity) -> bool = 0;
};

class Entity {
  DEFINE_DEFAULT_COPYABLE_TYPE(Entity);

 private:
  EntityId id_;

 public:
  Entity(const EntityId id) :
    id_(id) {}
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
