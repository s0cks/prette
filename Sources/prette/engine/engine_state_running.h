#ifndef PRT_ENGINE_STATE_RUNNING_H
#define PRT_ENGINE_STATE_RUNNING_H

#include "prette/common.h"
#include "prette/on_shutdown.h"
#include "prette/uv/uv_ticker.h"
#include "prette/engine/engine_state.h"

namespace prt::engine {
  class RunningState : public EngineState,
                       public ShutdownListener {
    friend class Engine;
    DEFINE_NON_COPYABLE_TYPE(RunningState);
  private:
    ShutdownListenerHandle shutdown_;
    rx::subscription on_tick_;
  protected:
    explicit RunningState(Engine* engine);

    void RunTick();
    void OnShutdown() override;
  public:
    ~RunningState() override = default;
    DECLARE_ENGINE_STATE(Running);
  };
}

#endif //PRT_ENGINE_STATE_RUNNING_H