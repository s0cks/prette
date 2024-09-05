#include "prette/gfx_mapped_buffer_scope.h"

namespace prt::gfx {
  uword MappedBufferScope::MapBuffer(const GLenum target, const Access access) {
    const auto ptr = glMapBuffer(target, access);
    CHECK_GL;
    DLOG(INFO) << "mapped " << target << " to " << ((const void*)ptr) << " (" << access << ")";
    return (uword) ptr;
  }

  void MappedBufferScope::UnmapBuffer(const GLenum target) {
    glUnmapBuffer(target);
    CHECK_GL;
  }
}