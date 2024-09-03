#include "prette/engine/engine_state.h"
#include "prette/engine/engine.h"
#include "prette/render/renderer.h"

namespace prt::engine {
  RunningState::RunningState(Engine* engine):
    EngineState(engine),
    shutdown_(engine->GetLoop(), this),
    on_tick_() {
    auto& ticker = engine->ticker_;
    on_tick_ = ticker.ToObservable()
      .subscribe([engine,this,&ticker](const uv::Tick& tick) {
        Publish<PreTickEvent>(engine);
        glfwPollEvents();
        Publish<TickEvent>(engine, tick, ticker.GetPreviousTick());
      });
  }

  RunningState::~RunningState() {
    
  }

  void RunningState::RunTick() {
    auto& loop = GetEngine()->GetLoop();
    loop.RunOnce();
  }

  void RunningState::RunRenderer() {
    const auto renderer = GetRenderer();
    PRT_ASSERT(renderer);
    renderer->Run();
  }

  void RunningState::Run() {
    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    engine->SetRunning(true);
    while(engine->IsRunning()) {
      RunTick();
      RunRenderer();
    }
    PRT_ASSERT(!engine->IsRunning());
  }

  void RunningState::Stop() {
    shutdown_.Call();
  }

  void RunningState::OnShutdown() {
    const auto renderer = GetRenderer();
    PRT_ASSERT(renderer);
    renderer->Stop();

    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    engine->Stop();

    engine->SetRunning(false);
  }
}