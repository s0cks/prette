#include "prette/transform.h"

#include "prette/thread_local.h"
#include "prette/engine/engine.h"

namespace prt {
  static ThreadLocal<TransformComponent> component_;

  static inline void
  Set(TransformComponent* component) {
    PRT_ASSERT(component);
    component_.Set(component);
  }

  void TransformComponent::Init() {
    DLOG(INFO) << "initializing....";
    Set(new TransformComponent());
  }

  TransformComponent* TransformComponent::Get() {
    return component_.Get();
  }
}