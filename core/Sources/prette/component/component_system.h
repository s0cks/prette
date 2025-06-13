#ifndef PRT_COMPONENT_SYSTEM_H
#define PRT_COMPONENT_SYSTEM_H

#include <absl/container/flat_hash_map.h>
#include <cstdint>
#include <gflags/gflags_declare.h>
#include <iterator>
#include <typeindex>

#include "prette/common.h"
#include "prette/component/component_event.h"
#include "prette/component/component_table.h"
#include "prette/system.h"
#include "prette/tick.h"

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
 private:
  absl::flat_hash_map<std::type_index, ComponentTable*> tables_{};

  ComponentSystem();

  template <class C>
  static inline auto GetIndex() -> std::type_index {
    return std::type_index(typeid(C));
  }

  template <class C>
  void RegisterComponent() {
    const auto index = GetIndex<C>();
    {
      const auto pos = tables_.find(index);
      if (pos != std::end(tables_)) {
        LOG(ERROR) << "cannot overwrite component: " << index.name();
        return;
      }
    }

    const auto [pos, success] = tables_.insert({index, new ComponentTableTemplate<C>()});
    LOG_IF(FATAL, !success) << "failed to register component: " << index.name();
  }

  template <class C>
  auto GetComponentTable() const -> ComponentTableTemplate<C>* {
    const auto pos = tables_.find(std::type_index(typeid(C)));
    if (pos != std::end(tables_))
      return (ComponentTableTemplate<C>*)(*pos);
    return nullptr;
  }

  void OnTick(const Tick& current, const Tick& previous) override;

 public:
  ~ComponentSystem() override;

  auto GetNumberOfComponentsRegistered() const -> uint64_t {
    return tables_.size();
  }

  auto GetSystemName() const -> const char* override {
    return kSystemName;
  }

 public:
  DECLARE_SYSTEM_TYPE(Component);
};
}  // namespace prt

#endif  // PRT_COMPONENT_SYSTEM_H
