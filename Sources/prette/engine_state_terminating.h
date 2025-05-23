#ifndef PRT_ENGINE_STATE_TERMINATING_H
#define PRT_ENGINE_STATE_TERMINATING_H

#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine_state.h> instead."
#endif  // PRT_ENGINE_STATE_H

#include <memory>

namespace prt {
class Engine;
class TerminatingState : public EngineState {
 protected:
  void EnterState(Engine* engine) override;

 public:
  TerminatingState() = default;
  ~TerminatingState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Terminating);

 public:
  static inline auto New() -> std::unique_ptr<TerminatingState> {
    return std::make_unique<TerminatingState>();
  }
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_TERMINATING_H
