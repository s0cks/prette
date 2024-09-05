#include "prette/gfx.h"

namespace prt::gfx {
  std::string GlError::ToString() const {
    std::stringstream ss;
    switch(GetCode()) {
#define DEFINE_TO_STRING(Name, GlValue)                     \
      case (GlValue):                                       \
        ss << "GlError " << (GlValue) << " - " << #Name;    \
        break;
      FOR_EACH_GL_ERROR(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default:
        ss << "Unknown GlError: " << GetCode();
        break;
    }
    return ss.str();
  }

  GlException::GlException(const GlError error):
    std::exception(),
    error_(error),
    message_(error.ToString()) {
    LOG(INFO) << "message: " << message_;
  }
}