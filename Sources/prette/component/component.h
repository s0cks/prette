#ifndef PRT_COMPONENT_H
#define PRT_COMPONENT_H

#include <cstdint>
#include <set>
#include <unordered_map>
#include <array>
#include <typeinfo>

#include "prette/gfx.h"
#include "prette/relaxed_atomic.h"

#include "prette/entity/entity_tracker.h"

#include "prette/component/component_id.h"
#include "prette/component/component_state.h"
#include "prette/component/component_events.h"

namespace prt {
  namespace component {
    class Component;
    class ComponentVisitor {
    protected:
      ComponentVisitor() = default;
    public:
      virtual ~ComponentVisitor() = default;
      virtual auto Visit(Component* component) -> bool = 0;
    };

    class Component {
      friend class Components;
    public:
      struct ComponentIdComparator {
        auto operator()(const Component* lhs, const Component* rhs) const -> bool {
          return lhs->GetComponentId() == rhs->GetComponentId();
        }
      };
    private:
      RelaxedAtomic<bool> registered_;
      RelaxedAtomic<ComponentId> id_;
      entity::FilteredEntityTracker tracker_;
      rx::subscription pre_init_sub_;

      inline void SetRegistered(const bool registered = true) {
        registered_ = registered;
      }

      inline void ClearRegistered() {
        return SetRegistered(false);
      }

      inline void SetComponentId(const ComponentId id) {
        id_ = id;
      }

      inline void ClearComponentId() {
        return SetComponentId(kInvalidComponentId);
      }

      inline void SetSignature(const entity::Signature& signature) {
        tracker_.SetSignature(signature);
      }
    protected:
      Component();
    public:
      virtual ~Component();
      virtual auto GetName() const -> const char* = 0;

      auto GetEntities() const -> const entity::EntitySet& {
        return tracker_.GetEntities();
      }

      auto GetSignature() const -> const entity::Signature& {
        return tracker_.GetSignature();
      }

      inline auto GetComponentId() const -> ComponentId {
        return (ComponentId) id_;
      }

      inline auto IsRegistered() const -> bool {
        return (bool) registered_;
      }
    };

    template<class S>
    class StatefulComponent : public Component {
      using State = ComponentState<S>;
      using StateSet = std::set<State *, ComponentStateBase::EntityIdComparator>;
      using StateObservable = rx::observable<State *>;
      using StateSupplier = std::function<S *()>;
    private:
      ComponentStateTable<S> states_;
    protected:
      StatefulComponent():
        Component(),
        states_() {
      }

      auto RemoveState(const EntityId id) -> bool {
        return states_.Remove(id);
      }
    public:
      ~StatefulComponent() override = default;

      auto states() const -> const ComponentStateTable<S>& {
        return states_;
      }

      auto HasState(const EntityId id) const -> bool {
        return states_.Has(id);
      }

      auto GetState(const EntityId id) const -> State* {
        return states_.Get(id);
      }

      auto GetOrCreateState(const EntityId id) -> State* {
        return states_.GetOrCreate(id);
      }

      auto CreateState(const EntityId id, const S* data = nullptr) -> State* {
        return states_.Create(id, (const uword) data);
      }

      auto GetStates() const -> StateObservable {
        return (StateObservable) states_;
      }
    };

    class Components {
      DEFINE_NON_INSTANTIABLE_TYPE(Components);
    public:
      static void Register(Component* component);
      static void ClearRegisteredComponents();
      static auto Visit(ComponentVisitor* vis) -> bool;
      static auto Get() -> rx::observable<Component*>;
      static auto OnEvent() -> rx::observable<ComponentEvent*>;

#define DEFINE_ON_EVENT(Name)                                                 \
      static inline auto                                                      \
      On##Name##Event() -> rx::observable<Name##Event*> {                     \
        return OnEvent()                                                      \
          .filter(Name##Event::Filter)                                        \
          .map(Name##Event::Cast);                                            \
      }
      FOR_EACH_COMPONENT_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#ifdef PRT_DEBUG
      static void PrintAll(const google::LogSeverity severity = google::INFO);
#endif //PRT_DEBUG
    };
  }

  using component::Components;
  using component::Component;
  using component::StatefulComponent;

#define DECLARE_COMPONENT(Name)                                       \
  public:                                                             \
    auto GetName() const -> const char*  override { return #Name; }
}

#endif //PRT_COMPONENT_H