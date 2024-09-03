#ifndef PRT_RENDER_PASS_GUIS_H
#define PRT_RENDER_PASS_GUIS_H

#include "prette/gui/gui_component.h"
#include "prette/render/render_pass.h"

#include "prette/program/program.h"
#include "prette/texture/texture.h"
#include "prette/camera/camera_ortho.h"

namespace prt::render {
  class RenderPassGuis : public RenderPass { //TODO: rename
  protected:
    Program* prog_;
    OrthoCamera* camera_;
    Texture* texture_;
  public:
    RenderPassGuis();
    ~RenderPassGuis() override = default;

    bool Accept(RenderPassVisitor* vis) override {
      return vis->Visit(this);
    }

    bool ShouldSkip() const override;
    void Render() override;
    DECLARE_RENDER_PASS(Guis);
  };
}

#endif //PRT_RENDER_PASS_GUIS_H