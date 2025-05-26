#include "prette/config/config_system.h"

#include "prette/assertions.h"
#include "prette/config/config.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<ConfigSystem> system_{};

ConfigSystem::ConfigSystem() {
  on_pre_init_ = OnPreInit([this](PreInitEvent* event) {
    LoadConfig();
  });
}

ConfigSystem::~ConfigSystem() {
  on_pre_init_.unsubscribe();
}

void ConfigSystem::Init() {
  ASSERT(!IsInitialized());
  system_ = new ConfigSystem();
  ASSERT(IsInitialized());
}

auto ConfigSystem::IsInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto ConfigSystem::Get() -> ConfigSystem* {
  ASSERT(IsInitialized());
  return system_;
}
}  // namespace prt