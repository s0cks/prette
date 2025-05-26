#ifndef PRT_ENGINE_STATE_TERMINATING_H
#define PRT_ENGINE_STATE_TERMINATING_H

#include "prette/engine/engine_state.h"

namespace prt {
class Engine;
class TerminatingState : public EngineState {
 protected:
  void EnterState(Engine* engine) override;

 public:
  TerminatingState() = default;
  ~TerminatingState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Terminating);
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_TERMINATING_H
