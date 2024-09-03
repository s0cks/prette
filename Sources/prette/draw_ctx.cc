#include "prette/draw_ctx.h"

#include "prette/color.h"
#include "prette/vbo/vbo_scope.h"
#include "prette/vao/vao_scope.h"
#include "prette/cull_face_scope.h"
#include "prette/program/program_scope.h"

namespace prt {
  DrawContext::DrawContext():
    vao_(Vao::New()) {
  }

  DrawContext::~DrawContext() {

  }

  void DrawContext::Draw(const gfx::DrawMode mode, const word first, const word count) const {
    vao::VaoBindScope vao(GetVao());
    InvertedCullFaceScope cull_face;
    InvertedDepthTestScope depth_test;
    gfx::ClearColorAndDepthBuffer();
    gfx::SetClearColor(kWhite);

    auto model = glm::mat4(1.0f);
    vbo::VboDrawScope draw_scope(GetVbo());
    program::ApplyProgramScope prog(prog_);
    prog.SetVP(GetCamera());
    prog.SetModelMatrix(model);
    draw_scope.Draw(mode, first, count);
  }

  void DrawContext::Draw(const gfx::DrawMode mode, const word first) const {
    return Draw(mode, first, GetVbo()->GetLength());
  }
}