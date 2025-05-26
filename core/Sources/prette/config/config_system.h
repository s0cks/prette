#ifndef PRT_CONFIG_SYSTEM_H
#define PRT_CONFIG_SYSTEM_H

#include "prette/config/config.h"
#include "prette/config/config_loader.h"
#include "prette/config/config_saver.h"
#include "prette/rx.h"

namespace prt {
class ConfigSystem {
 private:
  rx::subscription on_pre_init_{};
  Config config_{};

 private:
  inline void LoadConfig() {
    ConfigLoader::Load(&config_);
  }

  inline void SaveConfig() {
    ConfigSaver::Save(&config_);
  }

 public:
  ConfigSystem();
  ~ConfigSystem();

  auto config() -> Config& {
    return config_;
  }

 public:
  static void Init();
  static auto IsInitialized() -> bool;
  static auto Get() -> ConfigSystem*;
};
}  // namespace prt

#endif  // PRT_CONFIG_SYSTEM_H
