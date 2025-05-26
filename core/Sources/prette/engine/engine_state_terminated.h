#ifndef PRT_ENGINE_STATE_TERMINATED_H
#define PRT_ENGINE_STATE_TERMINATED_H

#include "prette/engine/engine_state.h"

namespace prt {
class Engine;
class TerminatedState : public EngineState {
 private:
  void EnterState(Engine* engine) override;
  void ExitState(Engine* engine) override;

 public:
  TerminatedState() = default;
  ~TerminatedState() override = default;
  DECLARE_ENGINE_STATE_TYPE(Terminated);
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_TERMINATED_H
