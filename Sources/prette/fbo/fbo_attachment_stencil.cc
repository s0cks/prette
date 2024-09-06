#include "prette/fbo/fbo_attachment.h"

namespace prt::fbo {
  std::string StencilAttachment::ToString() const {
    std::stringstream ss;
    ss << "StencilAttachment(";
    ss << ")";
    return ss.str();
  }

  int StencilAttachment::CompareTo(Attachment* rhs) const {
    NOT_IMPLEMENTED(ERROR); //TODO: implement
    return 0;
  }

  bool StencilAttachment::Equals(Attachment* rhs) const {
    NOT_IMPLEMENTED(ERROR); //TODO: implement
    return false;
  }

  StencilAttachment* StencilAttachment::New() {
    NOT_IMPLEMENTED(FATAL);//TODO: implement
    return nullptr;
  }
}