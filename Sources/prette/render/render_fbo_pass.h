#ifndef PRT_RENDER_FBO_PASS_H
#define PRT_RENDER_FBO_PASS_H

#include "prette/fbo/fbo.h"
#include "prette/fbo/fbo_scope.h"
#include "prette/program/program.h"
#include "prette/render/render_pass.h"

namespace prt::render {
  class RenderFboPass : public RenderPass {
  protected:
    Fbo* fbo_;
    Program* program_; 

    void Render() override;
  public:
    explicit RenderFboPass(Fbo* fbo):
      RenderPass(),
      fbo_(fbo),
      program_(Program::FromJson("program:textured_2d")) {
    }
    ~RenderFboPass() override = default;

    Order GetOrder() const override {
      return kLast;
    }

    const char* GetName() const override {
      return "RenderFbo";
    }
  };
}

#endif //PRT_RENDER_FBO_PASS_H