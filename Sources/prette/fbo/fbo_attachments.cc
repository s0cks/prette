#include "prette/fbo/fbo_attachments.h"

namespace prt::fbo {
  bool AttachmentSet::AttachmentComparator::operator()(Attachment* lhs, Attachment* rhs) const {
    PRT_ASSERT(lhs);
    PRT_ASSERT(rhs);
    return lhs->Equals(rhs);
  }

  void AttachmentSet::Insert(Attachment* attachment) {
    attachments_.insert(attachment);
  }

  void AttachmentSet::Remove(Attachment* attachment) {
    attachments_.erase(attachment);
  }

  uint64_t AttachmentSet::GetNumberOfAttachments() const {
    return attachments_.size();
  }

  rx::observable<Attachment*> AttachmentSet::ToObservable() const {
    return rx::observable<>::iterate(attachments_);
  }
}