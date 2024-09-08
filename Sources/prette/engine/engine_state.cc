#include "prette/engine/engine_state.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_events.h"

namespace prt::engine {
  void EngineState::PublishEvent(EngineEvent* event) {
    return GetEngine()->PublishEvent(event);
  }

  auto EngineState::OnEvent() const -> EngineEventObservable {
    return GetEngine()->OnEvent();
  }
}