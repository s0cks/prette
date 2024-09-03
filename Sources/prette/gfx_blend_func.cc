#include "prette/gfx_blend_func.h"

namespace prt::gfx {
  void BlendFunc::GetCurrent(BlendFunc& func) {
    glGetIntegerv(GL_BLEND_SRC_RGB, &func.src_.rgb_);
    CHECK_GL(FATAL);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &func.src_.alpha_);
    CHECK_GL(FATAL);
    glGetIntegerv(GL_BLEND_DST_RGB, &func.dst_.rgb_);
    CHECK_GL(FATAL);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &func.dst_.alpha_);
    CHECK_GL(FATAL);
  }

  void BlendFunc::Apply(const BlendFuncComponent& dst, const BlendFuncComponent& src) {
    glBlendFuncSeparate(src.rgb(), dst.rgb(), src.alpha(), dst.alpha());
    CHECK_GL(FATAL);
  }
}