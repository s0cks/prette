#ifndef PRT_ENTITY_SET_H
#define PRT_ENTITY_SET_H

#include <unordered_set>

#include "prette/rx.h"
#include "prette/entity/entity_id.h"

namespace prt::entity {
  class EntitySet {
  private:
    std::unordered_set<EntityId> ids_{};
  public:
    EntitySet() = default;
    virtual ~EntitySet() = default;

    auto begin() -> std::unordered_set<EntityId>::iterator {
      return ids_.begin();
    }

    auto begin() const -> std::unordered_set<EntityId>::const_iterator {
      return ids_.begin();
    }

    auto end() -> std::unordered_set<EntityId>::iterator {
      return ids_.begin();
    }

    auto end() const -> std::unordered_set<EntityId>::const_iterator {
      return ids_.end();
    }

    virtual auto Put(const EntityId id) -> bool {
      const auto result = ids_.insert(id);
      return result.second;
    }

    virtual auto Contains(const EntityId id) const -> bool {
      const auto pos = ids_.find(id);
      return pos != ids_.end();
    }

    virtual auto Remove(const EntityId id) -> bool {
      const auto pos = ids_.find(id);
      if(pos == ids_.end())
        return false;
      return ids_.erase(id) == 1;
    }

    explicit operator rx::observable<EntityId> () const {
      return rx::observable<>::iterate(ids_);
    }
  };
}

#endif //PRT_ENTITY_SET_H