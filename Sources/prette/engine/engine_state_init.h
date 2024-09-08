#ifndef PRT_ENGINE_STATE_INIT_H
#define PRT_ENGINE_STATE_INIT_H

#include "prette/engine/engine_state.h"

namespace prt::engine {
  class InitState : public EngineState {
    friend class Engine;
    DEFINE_NON_COPYABLE_TYPE(InitState);
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