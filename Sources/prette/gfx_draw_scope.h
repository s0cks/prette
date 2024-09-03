#ifndef PRT_GFX_DRAW_SCOPE_H
#define PRT_GFX_DRAW_SCOPE_H

#include "prette/gfx_draw_mode.h"

namespace prt::gfx {
  class DrawScope {
  protected:
    DrawScope() = default;
  public:
    virtual ~DrawScope() = default;
    virtual void Draw(const gfx::DrawMode mode) = 0;

#define DEFINE_DRAW(Name, _)                                                      \
    inline void Draw##Name() {                                                    \
      return Draw(gfx::k##Name##Mode);                                            \
    }
    FOR_EACH_DRAW_MODE(DEFINE_DRAW)
#undef DEFINE_DRAW
  };
}

#endif //PRT_GFX_DRAW_SCOPE_H