#ifndef PRT_DRAW_CTX_H
#define PRT_DRAW_CTX_H

#include "prette/vao/vao.h"
#include "prette/vbo/vbo.h"
#include "prette/ibo/ibo.h"
#include "prette/gfx_draw_mode.h"
#include "prette/program/program.h"
#include "prette/camera/camera_ortho.h"

namespace prt {
  class DrawContext {
  protected:
    Vao* vao_;
    Vbo* vbo_;
    Program* prog_;
    OrthoCamera* camera_;

    DrawContext();
  public:
    ~DrawContext();

    Vao* GetVao() const {
      return vao_;
    }

    Vbo* GetVbo() const {
      return vbo_;
    }

    Program* GetProgram() const {
      return prog_;
    }

    OrthoCamera* GetCamera() const {
      return camera_;
    }

    void Draw(const gfx::DrawMode mode, const word first, const word count) const;
    void Draw(const gfx::DrawMode mode, const word first = 0) const;

#define DEFINE_DRAW(Name, _)      \
    inline void Draw##Name(const word first, const word count) const {  \
      return Draw(gfx::k##Name##Mode, first, count);                    \
    }                                                                   \
    inline void Draw##Name(const word first = 0) {                      \
      return Draw(gfx::k##Name##Mode, first);                           \
    }
    FOR_EACH_DRAW_MODE(DEFINE_DRAW);
#undef DEFINE_DRAW
  };
}

#endif //PRT_DRAW_CTX_H