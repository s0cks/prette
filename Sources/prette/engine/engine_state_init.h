#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine/engine_state.h> instead."
#endif //PRT_ENGINE_STATE_H

#ifndef PRT_ENGINE_STATE_INIT_H
#define PRT_ENGINE_STATE_INIT_H

#include <vector>
#include "prette/uv/uv_handle.h"

namespace prt::engine {
  class InitState : public EngineState {
    friend class Engine;
  protected:
    explicit InitState(Engine* engine):
      EngineState(engine) {
    }
  public:
    ~InitState() override = default;
    DECLARE_ENGINE_STATE(Init);
  };
}

#endif //PRT_ENGINE_STATE_INIT_H