#ifndef PRT_CONFIG_MANAGER_H
#define PRT_CONFIG_MANAGER_H

#include "prette/config.h"
#include "prette/rx.h"

namespace prt {
class ConfigManager {
 private:
  rx::subscription on_pre_init_{};
  Config* config_ = nullptr;

 private:
  ConfigManager();

 public:
  ~ConfigManager();

  auto GetConfig() const -> Config* {
    return config_;
  }

  inline auto IsConfigLoaded() const -> bool {
    return GetConfig() != nullptr;
  }

 public:
  static void Init();
  static auto IsInitialized() -> bool;
  static auto Get() -> ConfigManager*;
};
}  // namespace prt

#endif  // PRT_CONFIG_MANAGER_H
