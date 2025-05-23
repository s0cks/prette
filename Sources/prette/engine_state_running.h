#ifndef PRT_ENGINE_STATE_RUNNING_H
#define PRT_ENGINE_STATE_RUNNING_H

#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine_state.h> instead."
#endif  // PRT_ENGINE_STATE_H

#include <memory>

#include "prette/rx.h"

namespace prt {
class Engine;
class RunningState : public EngineState {
  friend class Engine;

 private:
  rx::subscription on_tick_{};

 protected:
  void EnterState(Engine* engine) override;
  void OnTick(Engine* engine, const Tick& current, const Tick& previous) override;
  void ExitState(Engine* engine) override;

 public:
  RunningState() = default;
  ~RunningState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Running);

 public:
  static inline auto New() -> std::unique_ptr<RunningState> {
    return std::make_unique<RunningState>();
  }
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_RUNNING_H
