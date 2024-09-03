#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo.h"

namespace prt::vbo {
  void VboDrawScope::Draw(const gfx::DrawMode mode, const int64_t first, const int64_t count) const {
    DLOG(INFO) << "drawing " << count << " vertices starting @" << first;
    glDrawArrays(mode, first, count);
    CHECK_GL(FATAL);
  }

  void VboDrawScope::Draw(const gfx::DrawMode mode, const int64_t first) const {
    return Draw(mode, first, GetVbo()->GetLength());
  }
}