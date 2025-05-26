#include "prette/mouse/mouse_system.h"

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/settings/settings.h"
#include "prette/settings/settings_event.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<MouseSystem> system_{};

MouseSystem::MouseSystem() {
  on_loading_settings_ = OnLoadingSettings([this](LoadingSettingsEvent* event) {
    LoadSettings();
  });
  on_saving_settings_ = OnSavingSettings([this](SavingSettingsEvent* event) {
    SaveSettings();
  });
}

MouseSystem::~MouseSystem() {
  // do nothing?
}

void MouseSystem::LoadSettings() {
  NOT_IMPLEMENTED(ERROR);  // TODO: implement @s0cks
}

void MouseSystem::SaveSettings() {
  NOT_IMPLEMENTED(ERROR);  // TODO: implement @s0cks
}

void MouseSystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new MouseSystem();
}

auto MouseSystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto MouseSystem::GetSystem() -> MouseSystem* {
  ASSERT(IsSystemInitialized());
  return system_.Get();
}
}  // namespace prt