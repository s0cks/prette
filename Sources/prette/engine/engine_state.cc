#include "prette/engine/engine_state.h"
#include "prette/engine/engine.h"

namespace prt::engine {
  void EngineState::Publish(EngineEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = GetEngine()->GetSubscriber();
    subscriber.on_next(event);
  }
}