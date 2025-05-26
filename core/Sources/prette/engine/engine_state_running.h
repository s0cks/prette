#ifndef PRT_ENGINE_STATE_RUNNING_H
#define PRT_ENGINE_STATE_RUNNING_H

#include "prette/engine/engine_state.h"
#include "prette/next_tick.h"
#include "prette/rx.h"
#include "prette/tick.h"

namespace prt {
class Engine;
class RunningState : public EngineState {
  friend class Engine;

 private:
  rx::subscription on_tick_{};
  auto StartTicker(Engine* engine) -> rx::composite_subscription;
  void StopTicker(Engine* engine);
  void ClearNextTickListeners(const NextTickQueue& queue, const Tick& current, const Tick& previous);

 protected:
  void EnterState(Engine* engine) override;
  void OnTick(Engine* engine, const Tick& current, const Tick& previous) override;
  void ExitState(Engine* engine) override;

 public:
  RunningState() = default;
  ~RunningState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Running);
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_RUNNING_H
