#ifndef PRT_ENTITY_SET_H
#define PRT_ENTITY_SET_H

#include <unordered_set>

#include "prette/rx.h"
#include "prette/entity/entity_id.h"

namespace prt::entity {
  class EntitySet {
  protected:
    std::unordered_set<EntityId> ids_;
  public:
    EntitySet() = default;
    virtual ~EntitySet() = default;

    std::unordered_set<EntityId>::iterator begin() {
      return ids_.begin();
    }

    std::unordered_set<EntityId>::const_iterator begin() const {
      return ids_.begin();
    }

    std::unordered_set<EntityId>::iterator end() {
      return ids_.begin();
    }

    std::unordered_set<EntityId>::const_iterator end() const {
      return ids_.end();
    }

    virtual bool Put(const EntityId id) {
      const auto result = ids_.insert(id);
      return result.second;
    }

    virtual bool Contains(const EntityId id) const {
      const auto pos = ids_.find(id);
      return pos != ids_.end();
    }

    virtual bool Remove(const EntityId id) {
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