#ifndef PRT_RENDER_PASS_ROOT_H
#define PRT_RENDER_PASS_ROOT_H

#include "prette/color.h"
#include "prette/render/render_pass.h"

namespace prt::render {
  class RenderPassGuis;
  class RootRenderPass : public OrderedSequenceRenderPass {
  protected:
    Color clear_;
    RenderPassGuis* guis_;

    void Render() override;
  public:
    RootRenderPass();
    ~RootRenderPass() override = default;
  };
}

#endif //PRT_RENDER_PASS_ROOT_H