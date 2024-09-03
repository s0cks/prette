
#include "prette/render/render_pass_guis.h"

#include "prette/shape.h"
#include "prette/gui/gui_tree.h"
#include "prette/gui/gui_window.h"
#include "prette/gui/gui_context.h"

#include "prette/thread_local.h"
#include "prette/vao/vao_scope.h"

#include "prette/ibo/ibo_scope.h"
#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo_builder.h"

#include "prette/window/window.h"

#include "prette/cull_face_scope.h"

#include "prette/render/renderer.h"
#include "prette/render/render_fbo_pass.h"

#include "prette/texture/texture_scope.h"
#include "prette/program/program_scope.h"

namespace prt::render {
  bool RenderPassGuis::ShouldSkip() const {
    return gui::Tree::IsEmpty();
  }

  static ThreadLocal<gui::Context> context_;

  static inline gui::Context*
  GetContext() {
    if(context_)
      return context_.Get();
    const auto ctx = new gui::Context();
    context_.Set(ctx);
    return ctx;
  }

  RenderPassGuis::RenderPassGuis():
    RenderPass(),
    prog_(Program::FromJson("program:textured_2d")),
    texture_(Texture2d::New("texture:planks.png")),
    camera_(nullptr) {
    const auto target_res = GetTargetResolution();
    camera_ = OrthoCamera::New(0.0f, target_res.width() * 1.0f, target_res.height() * 1.0f, 0.0f);
  }

  void RenderPassGuis::Render() {
    const auto window = GetAppWindow();
    PRT_ASSERT(window);
    const auto ctx = GetContext();
    PRT_ASSERT(ctx);
    {
      // compute vertices && indices for roots
      gui::ComponentRenderer renderer(ctx);
      const auto num_roots = gui::Tree::GetNumberOfRoots();
      DLOG(INFO) << "rendering " << num_roots << " root components.....";
      LOG_IF(ERROR, !gui::Tree::VisitAllRoots(&renderer)) << "failed to render tree.";
      if(ctx->IsDirty())
        ctx->Commit();
    }

    {
      InvertedCullFaceScope cull_face;
      InvertedDepthTestScope depth_test;
      gfx::ClearColorAndDepthBuffer();
      gfx::SetClearColor(kWhite);

      texture::TextureBindScope<0> texture(texture_);

      vao::VaoBindScope vao(ctx->GetVao());
      auto model = glm::mat4(1.0f);
      vbo::VboDrawScope draw_scope(ctx->GetVbo());
      program::ApplyProgramScope prog(prog_);
      prog.Set("projection", camera_->GetProjection());
      prog.Set("view", camera_->GetView());
      prog.Set("model", model);
      prog.Set("tex", 0);
      draw_scope.DrawTriangles();
    }
  }
}