#include "prette/render/render_pass_root.h"

#include "prette/render/renderer.h"
#include "prette/render/render_pass_guis.h"


namespace prt::render {
  RootRenderPass::RootRenderPass():
    OrderedSequenceRenderPass(),
    clear_(Color(128, 128, 128, 255)),
    guis_(new RenderPassGuis()) {
    Append(guis_);
  }

  void RootRenderPass::Render() {
    // const auto resolution = render::GetTargetResolution();
    // glViewport(0, 0, resolution.width(), resolution.height());

    const auto renderer = GetRenderer();
    glPolygonMode(GL_FRONT_AND_BACK, renderer->GetMode());
    CHECK_GL;
    glClear(GL_COLOR_BUFFER_BIT);
    CHECK_GL;
    gfx::SetClearColor(clear_);
  }
}