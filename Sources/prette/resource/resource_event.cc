#include "prette/resource/resource_event.h"
#include <sstream>

namespace prt::resource {
#define DEFINE_TOSTRING(Name) \
  std::string Name##Event::ToString() const {           \
    std::stringstream ss;                               \
    ss << (*this);                                      \
    return ss.str();                                    \
  }
  FOR_EACH_RESOURCE_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
}