#include "prette/fbo/fbo_builder.h"

#include "prette/fbo/fbo_status.h"

namespace prt::fbo {
  Fbo* FboBuilder::Build() const {
    const auto id = GenerateFboId();
    PRT_ASSERT(IsValidFboId(id));
    Fbo::BindFbo(id);
    DLOG(INFO) << "attaching " << attachments_.GetNumberOfAttachments() << " attachments....";
    for(const auto& attachment : attachments_) {
      DLOG(INFO) << "attaching " << attachment->ToString() << "....";
      attachment->AttachTo();
    }

    FboStatus status;
    LOG_IF(FATAL, !status) << "failed to initialize framebuffer: " << status;
    Fbo::BindDefaultFbo();
    return Fbo::New(id, attachments_);
  }
}