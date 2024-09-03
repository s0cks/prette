#ifndef PRT_FBO_ATTACHMENTS_H
#define PRT_FBO_ATTACHMENTS_H

#include <set>
#include "prette/fbo/fbo_attachment.h"

namespace prt::fbo {
  class AttachmentSet {
    struct AttachmentComparator {
      bool operator() (Attachment* lhs, Attachment* rhs) const;
    };

    typedef std::set<Attachment*, AttachmentComparator> Attachments;
  protected:
    Attachments attachments_;
  public:
    AttachmentSet() = default;
    virtual ~AttachmentSet() = default;

    Attachments::const_iterator begin() const {
      return std::begin(attachments_);
    }

    Attachments::const_iterator end() const {
      return std::end(attachments_);
    }

    virtual void Insert(Attachment* attachment);
    virtual void Remove(Attachment* attachment);
    virtual uint64_t GetNumberOfAttachments() const;
    virtual rx::observable<Attachment*> ToObservable() const;

    inline rx::observable<ColorAttachment*> GetAllColorAttachments() const {
      return ToObservable()
        .filter([](Attachment* att) {
          return att->IsColorAttachment();
        })
        .map([](Attachment* att) {
          return att->AsColorAttachment();
        });
    }

    inline rx::observable<DepthAttachment*> GetAllDepthAttachments() const {
      return ToObservable()
        .filter([](Attachment* att) {
          return att->IsDepthAttachment();
        })
        .map([](Attachment* att) {
          return att->AsDepthAttachment();
        });
    }

    inline rx::observable<StencilAttachment*> GetAllStencilAttachments() const {
      return ToObservable()
        .filter([](Attachment* att) {
          return att->IsStencilAttachment();
        })
        .map([](Attachment* att) {
          return att->AsStencilAttachment();
        });
    }
  };
}

#endif //PRT_FBO_ATTACHMENTS_H