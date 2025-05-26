#ifndef PRT_SETTINGS_SYSTEM_H
#define PRT_SETTINGS_SYSTEM_H

#include "prette/rx.h"
#include "prette/settings/settings.h"

namespace prt {
class SettingsSystem {
 private:
  rx::subscription on_pre_init_{};
  rx::subscription on_terminated_{};
  Settings* settings_ = nullptr;

  SettingsSystem();  // TODO: reduce visibility

 public:
  ~SettingsSystem();

 public:
  static void InitSystem();
  static auto IsSystemInitialized() -> bool;
  static auto GetSystem() -> SettingsSystem*;
};
}  // namespace prt

#endif  // PRT_SETTINGS_SYSTEM_H
