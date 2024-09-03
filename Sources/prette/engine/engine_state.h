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
    const char* GetName() const override { return #Name; }                      \
    Name##State* As##Name##State() override { return this; }

  class EngineState : public State {
    friend class Engine;
  protected:
    Engine* engine_;

    explicit EngineState(Engine* engine):
      State(),
      engine_(engine) {
      PRT_ASSERT(engine);
    }

    void Publish(EngineEvent* event);

    template<class T, typename... Args>
    inline void Publish(Args... args) {
      T event(args...);
      return Publish((EngineEvent*) &event);
    }
  public:
    ~EngineState() override = default;

    inline Engine* GetEngine() const {
      return engine_;
    }

#define DEFINE_TYPE_CHECK(Name)                                                 \
    virtual Name##State* As##Name##State() { return nullptr; }                  \
    bool Is##Name##State() { return As##Name##State() != nullptr;  }
    FOR_EACH_ENGINE_STATE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
  };
}

#include "prette/engine/engine_state_init.h"
#include "prette/engine/engine_state_error.h"
#include "prette/engine/engine_state_running.h"
#include "prette/engine/engine_state_terminated.h"

#endif //PRT_ENGINE_STATE_H