#ifndef PRT_ENGINE_H
#define PRT_ENGINE_H

#include <units.h>
#include "prette/uv/utils.h"
#include "prette/uv/uv_loop.h"
#include "prette/uv/uv_ticker.h"
#include "prette/crash_report.h"
#include "prette/engine/engine_state.h"
#include "prette/engine/engine_events.h"

namespace prt::engine {
  class Engine : public EngineEventSource {
    friend class EngineState;
    friend class InitState;
    friend class ErrorState;
    friend class RunningState;
    friend class EngineTicker;
    friend class TerminatedState;
    DEFINE_NON_COPYABLE_TYPE(Engine);
  private:
    uv::Loop loop_;
    uv::Ticker ticker_;
    rx::subscription on_tick_;
    RelaxedAtomic<bool> running_;
    EngineState* state_;
    CrashReportCause cause_;
    EngineEventSubject events_;

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

    inline auto HasCause() const -> bool {
      return (bool) cause_;
    }

    auto GetCause() const -> const CrashReportCause& {
      return cause_;
    }

    void PublishEvent(EngineEvent* event) override;
  public:
    Engine();
    ~Engine() override;

    auto GetLoop() const -> const uv::Loop& {
      return loop_;
    }

    auto GetLoop() -> uv::Loop& {
      return loop_;
    }

    void Run();
    void Shutdown(const std::exception_ptr& cause = nullptr);

    template<typename E, typename... Args>
    inline void Shutdown(Args... args) {
      return Shutdown((const std::exception_ptr&) std::make_exception_ptr<E>(E(args...)));
    }

    virtual auto IsRunning() const -> bool {
      return (bool) running_;
    }

    inline auto HasState() const -> bool {
      return state_ != nullptr;
    }

    virtual auto GetState() const -> EngineState* {
      return state_;
    }

    auto OnEvent() const -> EngineEventObservable override;
  };

  void InitEngine();
  auto GetEngine() -> Engine*;
}

namespace prt {
  using engine::Engine;
  using engine::GetEngine;
}

#endif //PRT_ENGINE_H