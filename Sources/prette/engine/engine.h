#ifndef PRT_ENGINE_H
#define PRT_ENGINE_H

#include <units.h>
#include "prette/signals.h"
#include "prette/counter.h"
#include "prette/uv/utils.h"
#include "prette/uv/uv_loop.h"
#include "prette/crash_report.h"
#include "prette/engine/engine_state.h"
#include "prette/engine/engine_events.h"

namespace prt::engine {
  class Engine : public EngineEventPublisher {
    friend class EngineState;
    friend class InitState;
    friend class ErrorState;
    friend class RunningState;
    friend class EngineTicker;
    friend class TerminatedState;
  protected:
    uv::Loop loop_;
    uv::Ticker ticker_;
    rx::subscription on_tick_;
    RelaxedAtomic<bool> running_;
    EngineState* state_;
    CrashReportCause cause_;

    virtual void SetRunning(const bool running = true) {
      running_ = running;
    }

    inline void SetState(EngineState* state) {
      PRT_ASSERT(state);
      state_ = state;
    }

    void RunState(EngineState* state);

    template<class S, typename... Args>
    inline void RunState(Args... args) {
      S state(this, args...);
      return RunState(&state);
    }

    void Stop() {
      ticker_.Stop();
      loop_.Stop();
    }

    inline void SetCause(const std::exception_ptr& cause) {
      PRT_ASSERT(cause);
      cause_ = cause;
    }

    inline bool HasCause() const {
      return (bool) cause_;
    }

    const CrashReportCause& GetCause() const {
      return cause_;
    }
  public:
    Engine();
    ~Engine() override;

    const uv::Loop& GetLoop() const {
      return loop_;
    }

    uv::Loop& GetLoop() {
      return loop_;
    }

    void Run();
    void Shutdown(const std::exception_ptr& cause = nullptr);

    template<typename E, typename... Args>
    inline void Shutdown(Args... args) {
      return Shutdown((const std::exception_ptr&) std::make_exception_ptr<E>(E(args...)));
    }

    virtual bool IsRunning() const {
      return (bool) running_;
    }

    inline bool HasState() const {
      return state_ != nullptr;
    }

    virtual EngineState* GetState() const {
      return state_;
    }

#define DEFINE_ON_ENGINE_EVENT(Name)                                   \
    rx::observable<Name##Event*> On##Name##Event() const {             \
      return OnEvent()                                                 \
        .filter([](EngineEvent* event) {                               \
          return event->Is##Name##Event();                             \
        })                                                             \
        .map([](EngineEvent* event) {                                  \
          return event->As##Name##Event();                             \
        });                                                            \
    }
  FOR_EACH_ENGINE_EVENT(DEFINE_ON_ENGINE_EVENT)
#undef DEFINE_ON_ENGINE_EVENT
  };

  void InitEngine();
  Engine* GetEngine();
}

namespace prt {
  using engine::Engine;
  using engine::GetEngine;
}

#endif //PRT_ENGINE_H