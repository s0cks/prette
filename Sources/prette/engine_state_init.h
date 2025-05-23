#ifndef PRT_ENGINE_STATE_INIT_H
#define PRT_ENGINE_STATE_INIT_H

#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine_state.h> instead."
#endif  // PRT_ENGINE_STATE_H

#include <memory>

#include "prette/tick.h"

namespace prt {
class Engine;
class InitState : public EngineState {
 protected:
  virtual void EnterState(Engine* engine) override;
  virtual void ExitState(Engine* engine) override;

 public:
  InitState() = default;
  ~InitState() override = default;

  DECLARE_ENGINE_STATE_TYPE(Init);

 public:
  static inline auto New() -> std::unique_ptr<InitState> {
    return std::make_unique<InitState>();
  }
};
}  // namespace prt

#endif  // PRT_ENGINE_STATE_INIT_H
