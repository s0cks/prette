#ifndef PRT_ENGINE_STATE_PAUSED_H
#define PRT_ENGINE_STATE_PAUSED_H

#include "prette/engine/engine_state.h"

namespace prt {
class Engine;
class PausedState : public EngineState {
 protected:
  void EnterState(Engine* engine) override;

 public:
  PausedState() = default;
  ~PausedState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Paused);
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_PAUSED_H
