#ifndef PRT_ENGINE_STATE_H
#define PRT_ENGINE_STATE_H

#include <iostream>
#include <functional>
#include <glog/logging.h>

#include "prette/series.h"
#include "prette/state_machine.h"
#include "prette/engine/engine_events.h"

namespace prt::engine {
#define FOR_EACH_ENGINE_STATE(V) \
  V(Init)                        \
  V(Running)                     \
  V(Error)                       \
  V(Terminating)                 \
  V(Terminated)

  class Engine;
  class EngineState;
#define FORWARD_DECLARE_STATE(Name) class Name##State;
  FOR_EACH_ENGINE_STATE(FORWARD_DECLARE_STATE)
#undef FORWARD_DECLARE_STATE

#define DECLARE_ENGINE_STATE(Name)                                              \
  protected:                                                                    \
    void Run() override;                                                        \
    void Stop() override;                                                       \
  public:                                                                       \
    auto GetName() const -> const char* override { return #Name; }                      \
    auto As##Name##State() -> Name##State* override { return this; }

  class EngineState : public State,
                      public EngineEventSource {
    friend class Engine;
    DEFINE_NON_COPYABLE_TYPE(EngineState);
  private:
    Engine* engine_;
  protected:
    explicit EngineState(Engine* engine):
      State(),
      engine_(engine) {
      PRT_ASSERT(engine);
    }

    void PublishEvent(EngineEvent* event) override;
  public:
    ~EngineState() override = default;

    inline auto GetEngine() const -> Engine* {
      return engine_;
    }

    auto OnEvent() const -> EngineEventObservable override;

#define DEFINE_TYPE_CHECK(Name)                                                 \
    virtual auto As##Name##State() -> Name##State* { return nullptr; }          \
    auto Is##Name##State() -> bool { return As##Name##State() != nullptr;  }
    FOR_EACH_ENGINE_STATE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
  };
}

#endif //PRT_ENGINE_STATE_H