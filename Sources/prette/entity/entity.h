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
        size_t operator()(const Entity& k) const {
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
      constexpr Entity(const Entity& rhs):
        id_(rhs.id_) {
      }
      ~Entity() = default;

      EntityId id() const {
        return id_;
      }

      rx::observable<EntityEvent*> OnEvent() const;
#define DECLARE_ON_EVENT(Name) \
      rx::observable<Name##Event*> On##Name() const;
      FOR_EACH_ENTITY_EVENT(DECLARE_ON_EVENT)
#undef DECLARE_ON_EVENT

      constexpr operator EntityId() const {
        return id_;
      }

      void operator=(const Entity& rhs) {
        id_ = rhs.id_;
      }

      void operator=(const EntityId& rhs) {
        id_ = rhs;
      }

      bool operator==(const Entity& rhs) {
        return id_ == rhs.id_;
      }

      bool operator!=(const Entity& rhs) {
        return id_ != rhs.id_;
      }

      friend std::ostream& operator<<(std::ostream& stream, const Entity& rhs) {
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

    rx::observable<EntityEvent*> OnEvent();

    static inline rx::observable<EntityEvent*>
    OnEvent(const EntityId id) {
      return OnEvent()
        .filter(EntityEvent::FilterById(id));
    }

#define DEFINE_ON_EVENT(Name)                                                  \
    static inline rx::observable<Name##Event*>                                 \
    On##Name##Event() {                                                        \
      return OnEvent()                                                         \
        .filter(Name##Event::Filter)                                           \
        .map(Name##Event::Cast);                                               \
    }                                                                          \
    static inline rx::observable<Name##Event*>                                 \
    On##Name##Event(const EntityId id) {                                       \
      return OnEvent()                                                         \
        .filter(Name##Event::FilterBy(id))                                     \
        .map(Name##Event::Cast);                                               \
    }
    FOR_EACH_ENTITY_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

#define DEFINE_EVENT_LISTENER_INTERFACE(Name)                                    \
    class Name##EventListener {                                                  \
    protected:                                                                   \
      rx::subscription sub_;                                                     \
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
    FOR_EACH_ENTITY_EVENT(DEFINE_EVENT_LISTENER_INTERFACE)
#undef DEFINE_EVENT_LISTENER_INTERFACE
  }

  using entity::Signature;
  using entity::Entity;
}

#endif //PRT_ENTITY_H