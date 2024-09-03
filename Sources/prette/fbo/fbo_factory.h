#ifndef PRT_FBO_FACTORY_H
#define PRT_FBO_FACTORY_H

#include "prette/fbo/fbo.h"
#include "prette/fbo/fbo_attachment.h"
#include "prette/fbo/fbo_attachments.h"

namespace prt::fbo {
  class FboFactory {
  protected:
    AttachmentSet attachments_;

    Fbo* CreateFbo(const FboId id) const;
  public:
    FboFactory() = default;
    virtual ~FboFactory() = default;

    virtual void Attach(Attachment* attachment) {
      attachments_.Insert(attachment);
    }

    virtual rx::observable<Fbo*> Create(const int num = 1) const;
  };
}

#endif //PRT_FBO_FACTORY_H