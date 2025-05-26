#include "prette/settings/settings_system.h"

#include <filesystem>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/settings/settings.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<SettingsSystem> system_{};

SettingsSystem::SettingsSystem() {
  on_pre_init_ = OnPreInit([this](PreInitEvent* event) {
    settings_ = new Settings(fs::current_path() / "settings");
  });
  on_terminated_ = OnTerminated([this](TerminatedEvent* event) {
    delete settings_;
  });
}

SettingsSystem::~SettingsSystem() {
  on_pre_init_.unsubscribe();
  on_terminated_.unsubscribe();
}

void SettingsSystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new SettingsSystem();
}

auto SettingsSystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto SettingsSystem::GetSystem() -> SettingsSystem* {
  ASSERT(IsSystemInitialized());
  return system_.Get();
}
}  // namespace prt