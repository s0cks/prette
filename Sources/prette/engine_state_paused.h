#ifndef PRT_ENGINE_STATE_PAUSED_H
#define PRT_ENGINE_STATE_PAUSED_H

#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine_state.h> instead."
#endif  // PRT_ENGINE_STATE_H

#include <memory>

namespace prt {
class Engine;
class PausedState : public EngineState {
 protected:
  void EnterState(Engine* engine) override;

 public:
  PausedState() = default;
  ~PausedState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Paused);

 public:
  static inline auto New() -> std::unique_ptr<PausedState> {
    return std::make_unique<PausedState>();
  }
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_PAUSED_H
