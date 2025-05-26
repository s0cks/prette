#ifndef PRT_ENGINE_STATE_INIT_H
#define PRT_ENGINE_STATE_INIT_H

#include "prette/engine/engine_state.h"

namespace prt {
class Engine;
class InitState : public EngineState {
 protected:
  void EnterState(Engine* engine) override;
  void ExitState(Engine* engine) override;

 public:
  InitState() = default;
  ~InitState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Init);
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_INIT_H
