#include "prette/config_manager.h"

#include "prette/common.h"
#include "prette/config.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<ConfigManager> manager_{};

ConfigManager::ConfigManager() {
  on_pre_init_ = OnPreInit([this](PreInitEvent* event) {
    config_ = new Config(GetConfigPath());
  });
}

ConfigManager::~ConfigManager() {
  delete config_;
  on_pre_init_.unsubscribe();
}

void ConfigManager::Init() {
  ASSERT(!IsInitialized());
  manager_ = new ConfigManager();
  ASSERT(IsInitialized());
}

auto ConfigManager::IsInitialized() -> bool {
  return manager_.Get() != nullptr;
}

auto ConfigManager::Get() -> ConfigManager* {
  ASSERT(IsInitialized());
  return manager_;
}
}  // namespace prt