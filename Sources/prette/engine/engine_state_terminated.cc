#include "prette/engine/engine_state_terminated.h"
#include "prette/engine/engine.h"

namespace prt::engine {
  void TerminatingState::Run() {
    Publish<TerminatingEvent>(GetEngine());
  }

  void TerminatingState::Stop() {
    // do stuffs?
  }

  void TerminatedState::Run() {
    Publish<TerminatedEvent>(GetEngine());
  }

  void TerminatedState::Stop() {
    // do stuffs?
  }
}