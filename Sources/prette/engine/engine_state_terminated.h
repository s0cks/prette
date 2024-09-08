#ifndef PRT_ENGINE_STATE_TERMINATED_H
#define PRT_ENGINE_STATE_TERMINATED_H

#include "prette/engine/engine_state.h"

namespace prt::engine {
  class TerminatingState : public EngineState {
    friend class Engine;
    DEFINE_NON_COPYABLE_TYPE(TerminatingState);
  protected:
    explicit TerminatingState(Engine* engine):
      EngineState(engine) {
    }
  public:
    ~TerminatingState() override = default;
    DECLARE_ENGINE_STATE(Terminating);
  };

  class TerminatedState : public EngineState {
    friend class Engine;
    DEFINE_NON_COPYABLE_TYPE(TerminatedState);
  protected:
    explicit TerminatedState(Engine* engine):
      EngineState(engine) {
    }
  public:
    ~TerminatedState() override = default;
    DECLARE_ENGINE_STATE(Terminated);
  };
}

#endif //PRT_ENGINE_STATE_TERMINATED_H