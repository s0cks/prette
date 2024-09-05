#ifndef PRT_IBO_SCOPE_H
#error "Please #include <prette/ibo/ibo_scope.h> instead."
#endif //PRT_IBO_SCOPE_H

#ifndef PRT_IBO_DRAW_SCOPE_H
#define PRT_IBO_DRAW_SCOPE_H

#include "prette/ibo/ibo_scope.h"
#include "prette/gfx_draw_scope.h"

namespace prt::ibo {
  class IboDrawScope : public IboBindScope,
                       public gfx::DrawScope {
  public:
    explicit IboDrawScope(Ibo* ibo):
      IboBindScope(ibo) {
    }
    ~IboDrawScope() override = default;

    void Draw(const gfx::DrawMode mode, const word count, const GLvoid* indices = nullptr) {
      glDrawElements(mode, count, GetIbo()->GetGlType(), indices);
      CHECK_GL;
    }

    void Draw(const gfx::DrawMode mode) override {
      return Draw(mode, GetIbo()->GetLength());
    }

#define DEFINE_DRAW(Name, _)                                                      \
    inline void Draw##Name(const word count, const GLvoid* indices = 0) {         \
      return Draw(gfx::k##Name##Mode, count, indices);                            \
    }                                                                             \
    FOR_EACH_DRAW_MODE(DEFINE_DRAW)
#undef DEFINE_DRAW
  };
}

#endif //PRT_IBO_DRAW_SCOPE_H