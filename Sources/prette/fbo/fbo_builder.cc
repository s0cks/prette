#include "prette/fbo/fbo_builder.h"

namespace prt::fbo {
  Fbo* FboBuilder::Build() const {
    const auto id = GenerateFboId();
    PRT_ASSERT(IsValidFboId(id));
    PRT_ASSERT(!attachments_.IsEmpty());

    Fbo::BindFbo(id);
    DLOG(INFO) << "attaching " << attachments_.GetNumberOfAttachments() << " attachments....";
    for(const auto& attachment : attachments_) {
      DLOG(INFO) << "attaching " << attachment->ToString() << "....";
      attachment->AttachTo();
    }

    FboStatus status;
    if(!status)
      throw FboCreationException(status);
    Fbo::BindDefaultFbo();
    return Fbo::New(id, attachments_);
  }
}