#ifndef PRT_RENDER_PASS_EXECUTOR_H
#define PRT_RENDER_PASS_EXECUTOR_H

#include "prette/render/render_pass.h"

namespace prt::render {
  class RenderPassExecutor : public RenderPassVisitor {
  protected:
    bool Visit(RenderPass* pass) override;
  public:
    RenderPassExecutor() = default;
    ~RenderPassExecutor() override = default;
    virtual bool ExecuteRenderPass(RenderPass* pass);
  public:
    static inline bool
    Execute(RenderPass* pass) {
      RenderPassExecutor executor;
      return executor.ExecuteRenderPass(pass);
    }
  };
}

#endif //PRT_RENDER_PASS_EXECUTOR_H