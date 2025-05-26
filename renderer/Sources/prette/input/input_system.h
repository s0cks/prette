#ifndef PRT_INPUT_SYSTEM_H
#define PRT_INPUT_SYSTEM_H

#include "prette/rx.h"

namespace prt {
class InputSystem {
 private:
  rx::subscription on_pre_frame_{};

  void PreFrame();

 public:
  InputSystem();
  ~InputSystem() {
    on_pre_frame_.unsubscribe();
  }

 public:
  static void Init();
  static auto IsInitialized() -> bool;
  static auto Get() -> InputSystem*;
};
}  // namespace prt

#endif  // PRT_INPUT_SYSTEM_H
