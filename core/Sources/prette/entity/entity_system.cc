#include "prette/entity/entity_system.h"

#include "prette/assertions.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<EntitySystem> system_{};

EntitySystem::EntitySystem() {}

EntitySystem::~EntitySystem() {}

void EntitySystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new EntitySystem();
}

auto EntitySystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto EntitySystem::GetSystem() -> EntitySystem* {
  ASSERT(IsSystemInitialized());
  return system_.Get();
}
}  // namespace prt