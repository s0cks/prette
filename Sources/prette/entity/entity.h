#ifndef PRT_ENTITY_H
#define PRT_ENTITY_H

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <array>
#include <queue>
#include <bitset>
#include <set>
#include <glog/logging.h>

#include "prette/rx.h"
#include "prette/common.h"

#include "prette/entity/entity_id.h"
#include "prette/entity/entity_set.h"
#include "prette/entity/entity_events.h"
#include "prette/entity/entity_signature.h"

#include "prette/component/component_id.h"
#include "prette/component/component_state.h"

namespace prt {
  namespace entity {
    class Entity {
      struct HashFunction {
        auto operator()(const Entity& k) const -> size_t {
          return k.id();
        }
      };
    private:
      EntityId id_;

      void UpdateSignature(const ComponentId id, const bool value) const;

      inline void
      AddToSignature(const ComponentId id) const {
        return UpdateSignature(id, true);
      }

      inline void
      RemoveFromSignature(const ComponentId id) const {
        return UpdateSignature(id, false);
      }
    public:
      constexpr Entity(const EntityId id = kInvalidEntityId):
        id_(id) {
      }
      constexpr Entity(const Entity& rhs) = default;
      ~Entity() = default;

      auto id() const -> EntityId {
        return id_;
      }

      auto OnEvent() const -> rx::observable<EntityEvent*>;
#define DECLARE_ON_EVENT(Name) \
      auto On##Name() const -> rx::observable<Name##Event*>;
      FOR_EACH_ENTITY_EVENT(DECLARE_ON_EVENT)
#undef DECLARE_ON_EVENT

      constexpr operator EntityId() const {
        return id_;
      }

      auto operator=(const Entity& rhs) -> Entity& = default;

      auto operator==(const Entity& rhs) -> bool {
        return id_ == rhs.id_;
      }

      auto operator!=(const Entity& rhs) -> bool {
        return id_ != rhs.id_;
      }

      friend auto operator<<(std::ostream& stream, const Entity& rhs) -> std::ostream& {
        stream << "Entity(";
        stream << "id=" << rhs.id_;
        stream << ")";
        return stream;
      }

      //TODO:
      // template<typename T>
      // ComponentState<T> AddComponent(const T& component) const {
      //   auto state = ComponentState<T>(component);
      //   if(!T::PutState((*this), state)) {
      //     LOG(ERROR) << "failed to put " << state << " for " << (*this);
      //     return state;
      //   }
      //   AddToSignature(T::GetComponentId());
      //   return state;
      // }

      template<typename T>
      void RemoveComponent() const {
        LOG_IF(ERROR, !T::RemoveState(*this)) << "failed to remove state for " << *this;
        return RemoveFromSignature(T::GetComponentId());
      }
    };

    auto OnEvent() -> rx::observable<EntityEvent*>;

    static inline auto
    OnEvent(const EntityId id) -> rx::observable<EntityEvent*> {
      return OnEvent()
        .filter(EntityEvent::FilterById(id));
    }

#define DEFINE_ON_EVENT(Name)                                                  \
    static inline auto                                                         \
    On##Name##Event() -> rx::observable<Name##Event*> {                        \
      return OnEvent()                                                         \
        .filter(Name##Event::Filter)                                           \
        .map(Name##Event::Cast);                                               \
    }                                                                          \
    static inline auto                                                         \
    On##Name##Event(const EntityId id) -> rx::observable<Name##Event*> {       \
      return OnEvent()                                                         \
        .filter(Name##Event::FilterBy(id))                                     \
        .map(Name##Event::Cast);                                               \
    }
    FOR_EACH_ENTITY_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#define DEFINE_EVENT_LISTENER_INTERFACE(Name)                                    \
    class Name##EventListener {                                                  \
    private:                                                                     \
      rx::subscription sub_;                                                     \
    protected:                                                                   \
      Name##EventListener():                                                     \
        sub_() {                                                                 \
        sub_ = On##Name##Event()                                                 \
          .subscribe([this](Name##Event* event) {                                \
            return On##Name(event);                                              \
          });                                                                    \
      }                                                                          \
      virtual void On##Name(Name##Event* event) = 0;                             \
    public:                                                                      \
      virtual ~Name##EventListener() {                                           \
        sub_.unsubscribe();                                                      \
      }                                                                          \
    };
    FOR_EACH_ENTITY_EVENT(DEFINE_EVENT_LISTENER_INTERFACE);
#undef DEFINE_EVENT_LISTENER_INTERFACE
  }

  using entity::Signature;
  using entity::Entity;
}

#endif //PRT_ENTITY_H