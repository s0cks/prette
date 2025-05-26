#ifndef PRT_MOUSE_SYSTEM_H
#define PRT_MOUSE_SYSTEM_H

#include "prette/mouse/mouse_settings.h"
#include "prette/rx.h"

namespace prt {
class MouseSystem {
  static constexpr const auto kSystemName = "Mouse";

 private:
  rx::subscription on_loading_settings_{};
  rx::subscription on_saving_settings_{};
  MouseSettings settings_{};

  MouseSystem();

  void LoadSettings();
  void SaveSettings();

 public:
  ~MouseSystem();

 public:
  static void InitSystem();
  static auto IsSystemInitialized() -> bool;
  static auto GetSystem() -> MouseSystem*;
};
}  // namespace prt

#endif  // PRT_MOUSE_SYSTEM_H
