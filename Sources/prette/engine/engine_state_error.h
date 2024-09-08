#ifndef PRT_ENGINE_STATE_ERROR_H
#define PRT_ENGINE_STATE_ERROR_H

#include "prette/engine/engine_state.h"

namespace prt::engine {
  class ErrorState : public EngineState {
    friend class Engine;
    DEFINE_NON_COPYABLE_TYPE(ErrorState);
  protected:
    explicit ErrorState(Engine* engine);
  public:
    ~ErrorState() override = default;
    DECLARE_ENGINE_STATE(Error);
  };
}

#endif //PRT_ENGINE_STATE_ERROR_H