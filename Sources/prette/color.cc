#include "prette/color.h"

#include "prette/gfx.h"

namespace prt {
  namespace gfx {
    void SetClearColor(const Color& c) {
      glClearColor(c[0] / 255.0f, c[1] / 255.0f, c[2] / 255.0f, c[3] / 255.0f);
      CHECK_GL(FATAL);
    }
  }
}