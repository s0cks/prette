#include "prette/fbo/fbo_attachment.h"

namespace prt::fbo {
  std::string DepthAttachment::ToString() const {
    std::stringstream ss;
    ss << "DepthAttachment(";
    ss << ")";
    return ss.str();
  }

  int DepthAttachment::CompareTo(Attachment* rhs) const {
    NOT_IMPLEMENTED(ERROR); //TODO: implement
    return 0;
  }

  bool DepthAttachment::Equals(Attachment* rhs) const {
    NOT_IMPLEMENTED(ERROR); //TODO: implement
    return false;
  }

  DepthAttachment* DepthAttachment::New() {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }
}