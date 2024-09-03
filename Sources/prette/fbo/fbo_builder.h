#ifndef PRT_FBO_BUILDER_H
#define PRT_FBO_BUILDER_H

#include "prette/fbo/fbo.h"
#include "prette/builder.h"

namespace prt::fbo {
  class FboBuilder : public BuilderTemplate<Fbo> {
  private:
    AttachmentSet attachments_;
  public:
    FboBuilder() = default;
    ~FboBuilder() override = default;

    Fbo* Build() const override;
  };
}

#endif //PRT_FBO_BUILDER_H