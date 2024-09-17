#ifndef PRT_COMPONENT_STATE_H
#define PRT_COMPONENT_STATE_H

#include <set>
#include <functional>
#include <glog/logging.h>

#include "prette/rx.h"
#include "prette/common.h"
#include "prette/entity/entity_id.h"

namespace prt::component {
  class ComponentStateBase {
  public:
    struct EntityIdComparator {
      auto operator()(const ComponentStateBase* lhs, const ComponentStateBase* rhs) const -> bool {
        return lhs->entity() == rhs->entity();
      }
    };
  private:
    EntityId entity_;
  protected:
    explicit ComponentStateBase(const EntityId entity):
      entity_(entity) {
    }
  public:
    virtual ~ComponentStateBase() = default;
    virtual auto data() const -> uword = 0;

    auto entity() const -> EntityId {
      return entity_;
    }
  };

  template<class S>
  class ComponentState : public ComponentStateBase {
  private:
    uword data_;
  public:
    ComponentState(const EntityId id, const uword data):
      ComponentStateBase(id),
      data_(data) {
    }
    ~ComponentState() override = default;

    auto data() const -> uword override {
      return data_;
    }

    auto Get() const -> S* {
      return (S*) data();
    }

    auto operator->() const -> S* {
      return Get();
    }

    auto operator*() const -> const S& {
      return *Get();
    }
  };

  template<class S>
  class ComponentStateTable {
    using State = ComponentState<S>;
    using StateSet = std::set<State *, ComponentStateBase::EntityIdComparator>;
    using StateSupplier = std::function<State *>;
  private:
    class ComponentStateKey : public ComponentStateBase {
    public:
      explicit ComponentStateKey(const EntityId id):
        ComponentStateBase(id) {
      }
      ~ComponentStateKey() override = default;

       auto data() const -> uword override {
        return 0;
      }
    };
  private:
    StateSet states_;
  protected:
    auto GetState(const EntityId id) const -> ComponentStateBase* {
      for(const auto state : states_) {
        if(state->entity() == id)
          return state;
      }
      return nullptr;
    }

    inline auto PutState(State* state) -> ComponentStateBase* {
      const auto [iter,success] = states_.insert(state);
      LOG_IF(ERROR, !success) << "failed to insert ComponentState into ComponentStateTable.";
      return state;
    }
  public:
    ComponentStateTable() = default;
    virtual ~ComponentStateTable() = default;

    auto begin() const -> typename StateSet::const_iterator {
      return states_.begin();
    }

    auto end() const -> typename StateSet::const_iterator {
      return states_.end();
    }

    virtual void Remove(const EntityId id) {
      NOT_IMPLEMENTED(FATAL);//TODO: implement
    }

    virtual auto Has(const EntityId id) const -> bool {
      return GetState(id) != nullptr;
    }

    virtual auto Create(const EntityId id, const uword data = 0) -> State* {
      return (State*) PutState(new State(id, data));
    }

    virtual auto GetOrCreate(const EntityId id) -> State* {
      const auto state = (State*) GetState(id);
      return state ? state : Create(id);
    }

    explicit operator rx::observable<State*> () const {
      return rx::observable<>::iterate(states_);
    }
  };
}

#endif //PRT_COMPONENT_STATE_H