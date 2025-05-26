
#include "prette/engine/engine_state_running.h"

#include <chrono>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/engine/engine_state.h"
#include "prette/next_tick.h"
#include "prette/system.h"
#include "prette/system_ticker.h"
#include "prette/tick.h"

namespace prt {
#define __ engine->

auto RunningState::StartTicker(Engine* engine) -> rx::composite_subscription {
  __ StartTicker();
  return __ OnTick().subscribe([this, engine](const Tick& tick) {
    OnTick(engine, tick, engine->GetCurrentTick());
  });
}

void RunningState::StopTicker(Engine* engine) {
  on_tick_.unsubscribe();
  __ StopTicker();
}

ENGINE_STATE_ENTER_F(Running) {
  on_tick_ = StartTicker(engine);
  Engine::PublishRunningEvent();
  __ GetLoop().RunDefault();
}

void RunningState::ClearNextTickListeners(const NextTickQueue& queue, const Tick& current, const Tick& previous) {
  DVLOG(3) << "ticking " << queue.size() << " NextTickCallbacks....";
  uint64_t num_ticked = 0;
  PRT_PROFILING_BEGIN(all);
  for (const auto& next_callback : queue) {
    ASSERT(next_callback);
    PRT_PROFILING_BEGIN(next_callback);
    next_callback(current, previous);
    PRT_PROFILING_END(next_callback);
    num_ticked++;
    DVLOG(2) << "ticked NextTickCallback in "
             << std::chrono::duration_cast<std::chrono::milliseconds>(next_callback_duration).count() << "ms.";
  }
  PRT_PROFILING_END(all);
  DVLOG_IF(2, num_ticked > 0) << "ticked " << num_ticked << "/" << queue.size() << " NextTickCallbacks in "
                              << std::chrono::duration_cast<std::chrono::milliseconds>(all_duration).count() << "ms.";
}

ENGINE_STATE_TICK_F(Running) {
  // pre-tick
  Engine::PublishPreTickEvent();

  // tick-logic
  SystemTicker::TickAll(__ GetSystemsList(), current, previous);
  // next-tick listeners
  ClearNextTickListeners(__ GetNextTickQueue(), current, previous);
  // tick-event susbcribers
  Engine::PublishTickEvent(current, previous);

  // post-tick
  Engine::PublishPostTickEvent(current);
}

ENGINE_STATE_EXIT_F(Running) {
  StopTicker(engine);
}

#undef __
}  // namespace prt