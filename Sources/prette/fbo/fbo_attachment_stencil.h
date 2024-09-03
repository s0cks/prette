#ifndef PRT_FBO_ATTACHMENT_H
#error "Please #include <prette/fbo/fbo_attachment.h> instead."
#endif //PRT_FBO_ATTACHMENT_H

#ifndef PRT_FBO_ATTACHMENT_STENCIL_H
#define PRT_FBO_ATTACHMENT_STENCIL_H

#include "prette/fbo/fbo_attachment.h"

namespace prt::fbo {
  class StencilAttachment : public Attachment {
  protected:
    StencilAttachment() = default;
  public:
    ~StencilAttachment() override = default;
    DECLARE_FBO_ATTACHMENT(Stencil);
  public:
    static StencilAttachment* New();
  };
}

#endif //PRT_FBO_ATTACHMENT_STENCIL_H