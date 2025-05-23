#ifndef PRT_ENGINE_STATE_TERMINATED_H
#define PRT_ENGINE_STATE_TERMINATED_H

#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine_state.h> instead."
#endif  // PRT_ENGINE_STATE_H

#include <memory>

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

 public:
  static inline auto New() -> std::unique_ptr<TerminatedState> {
    return std::make_unique<TerminatedState>();
  }
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_TERMINATED_H
