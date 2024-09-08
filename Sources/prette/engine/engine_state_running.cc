#include "prette/engine/engine_state_running.h"

#include "prette/gfx.h"
#include "prette/engine/engine.h"

namespace prt::engine {
  RunningState::RunningState(Engine* engine):
    EngineState(engine),
    shutdown_(engine->GetLoop(), this),
    on_tick_() {
    auto& ticker = engine->ticker_;
    //TODO: slicing
    on_tick_ = ticker.ToObservable() // NOLINT(cppcoreguidelines-slicing)
      .subscribe([engine,this,&ticker](const uv::Tick& tick) {
        Publish<PreTickEvent>(engine);
        glfwPollEvents();
        Publish<TickEvent>(engine, tick, ticker.GetPreviousTick());
      });
  }

  void RunningState::RunTick() {
    auto& loop = GetEngine()->GetLoop();
    loop.RunOnce();
  }

  void RunningState::Run() {
    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    engine->SetRunning(true);
    while(engine->IsRunning()) {
      RunTick();
    }
    PRT_ASSERT(!engine->IsRunning());
  }

  void RunningState::Stop() {
    shutdown_.Call();
  }

  void RunningState::OnShutdown() {
    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    engine->Stop();

    engine->SetRunning(false);
  }
}