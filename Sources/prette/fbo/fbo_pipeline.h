#ifndef PRT_FBO_PIPELINE_H
#define PRT_FBO_PIPELINE_H

#include "prette/color.h"
#include "prette/fbo/fbo.h"
#include "prette/pipeline.h"

#include "prette/program/program.h"

namespace prt {
  namespace fbo {
    class FboPipeline : public Pipeline {
    protected:
      const Fbo* fbo_;

      explicit FboPipeline(const Fbo* fbo):
        Pipeline(),
        fbo_(fbo) {
      }
    public:
      ~FboPipeline() override = default;

      const Fbo* GetFbo() const {
        return fbo_;
      }
    };

    class RenderFboPipeline : public FboPipeline {
    protected:
      Color clear_color_;
      GLbitfield clear_flags_;
      ProgramRef shader_;

      explicit RenderFboPipeline(const Fbo* fbo):
        FboPipeline(fbo) {
      }

      bool Apply() override;
    public:
      ~RenderFboPipeline() override = default;
      
      const char* GetName() const override {
        return "RenderFbo";
      }
    public:
      static RenderFboPipeline* New(const Fbo* fbo);
    };
  }
  using fbo::FboPipeline;
}

#endif //PRT_FBO_PIPELINE_H