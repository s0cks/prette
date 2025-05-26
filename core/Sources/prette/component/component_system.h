#ifndef PRT_COMPONENT_SYSTEM_H
#define PRT_COMPONENT_SYSTEM_H

#include "prette/component/component_event.h"
#include "prette/system.h"

namespace prt {
auto GetComponentEventObservable() -> ComponentEventObservable;

template <typename... ArgN>
static inline auto OnComponentEvent(ArgN... args) -> rx::composite_subscription {
  return GetComponentEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                               \
  auto Get##Name##EventObservable()->Name##EventObservable; \
  template <typename... ArgN>                               \
  static inline auto On##Name##Event(ArgN... args) {        \
    return Get##Name##EventObservable().subscribe(args...); \
  }
FOR_EACH_COMPONENT_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

class ComponentSystem : public SystemTemplate<kNormal> {
  ComponentSystem();

 public:
  ~ComponentSystem();

  DECLARE_SYSTEM_TYPE(Component);
};
}  // namespace prt

#endif  // PRT_COMPONENT_SYSTEM_H
