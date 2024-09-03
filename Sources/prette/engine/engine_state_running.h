#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine/engine_state.h> instead."
#endif //PRT_ENGINE_STATE_H

#ifndef PRT_ENGINE_STATE_RUNNING_H
#define PRT_ENGINE_STATE_RUNNING_H

#include "prette/on_shutdown.h"
#include "prette/uv/uv_ticker.h"

namespace prt::engine {
  class RunningState : public EngineState,
                       public ShutdownListener {
    friend class Engine;
  protected:
    ShutdownListenerHandle shutdown_;
    rx::subscription on_tick_;

    explicit RunningState(Engine* engine);

    void RunTick();
    void RunRenderer();

    void OnShutdown() override;
  public:
    ~RunningState() override;
    DECLARE_ENGINE_STATE(Running);
  };
}

#endif //PRT_ENGINE_STATE_RUNNING_H