#include "prette/vbo/vbo_finalizer.h"
#include "prette/vbo/vbo.h"

namespace prt::vbo {
  void VboFinalizer::Finalize(Vbo* vbo) {
    PRT_ASSERT(vbo);
    DLOG(INFO) << "finalizing: " << vbo;
  }
}