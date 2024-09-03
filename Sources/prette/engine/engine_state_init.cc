#include "prette/engine/engine_state.h"
#include "prette/engine/engine.h"
#include "prette/uv/utils.h"

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