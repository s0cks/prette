#include "prette/engine/engine_state_init.h"
#include "prette/engine/engine.h"

namespace prt::engine {
  void InitState::Run() {
    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    Publish<PreInitEvent>(engine);
    DLOG(INFO) << "initializing engine....";
    Publish<PostInitEvent>(engine);
  }

  void InitState::Stop() {
    // do stuffs?
  }
}