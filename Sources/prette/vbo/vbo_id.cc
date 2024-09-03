#include "prette/vbo/vbo_id.h"

namespace prt::vbo {
  void GenerateVboIds(VboId* ids, const int num) {
    glGenBuffers(num, &ids[0]);
    CHECK_GL(FATAL);
  }
}