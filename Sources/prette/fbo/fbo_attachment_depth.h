#ifndef PRT_FBO_ATTACHMENT_H
#error "Please #include <prette/fbo/fbo_attachment.h> instead."
#endif //PRT_FBO_ATTACHMENT_H

#ifndef PRT_FBO_ATTACHMENT_DEPTH_H
#define PRT_FBO_ATTACHMENT_DEPTH_H

#include "prette/fbo/fbo_attachment.h"

namespace prt::fbo {
  class DepthAttachment : public Attachment {
  protected:
    DepthAttachment() = default;
  public:
    ~DepthAttachment() override = default;
    DECLARE_FBO_ATTACHMENT(Depth);
  public:
    static DepthAttachment* New();
  };
}

#endif //PRT_FBO_ATTACHMENT_DEPTH_H