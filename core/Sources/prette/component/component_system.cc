#include "prette/component/component_system.h"

#include "prette/common.h"
#include "prette/thread_local.h"
#include "prette/tick.h"

namespace prt {
ComponentSystem::ComponentSystem() = default;

ComponentSystem::~ComponentSystem() = default;

static ThreadLocal<ComponentSystem> system_{};

void ComponentSystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new ComponentSystem();
}

auto ComponentSystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto ComponentSystem::GetSystem() -> ComponentSystem* {
  return system_;
}

void ComponentSystem::OnTick(const Tick& current, const Tick& previous) {
  // do nothing
}
}  // namespace prt