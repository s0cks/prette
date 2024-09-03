#include "prette/fbo/fbo_pipeline.h"

namespace prt::fbo {
  RenderFboPipeline* RenderFboPipeline::New(const Fbo* fbo) {
    return new RenderFboPipeline(fbo);
  }

  bool RenderFboPipeline::Apply() {
    NOT_IMPLEMENTED(FATAL);//TODO: implement
    return false;
  }
}