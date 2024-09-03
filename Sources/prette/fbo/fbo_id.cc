#include "prette/fbo/fbo_id.h"

namespace prt::fbo {
  void GenerateFboIds(FboId* ids, const int num_ids) {
    glGenFramebuffers(num_ids, &ids[0]);
    CHECK_GL(FATAL);
  }
}