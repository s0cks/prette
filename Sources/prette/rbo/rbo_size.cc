#include "prette/rbo/rbo_size.h"

namespace prt::rbo {
  int32_t GetMaxRboSize() {
    GLint max_size;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_MAX_RENDERBUFFER_SIZE, &max_size);
    CHECK_GL;
    return static_cast<int32_t>(max_size);
  }
}