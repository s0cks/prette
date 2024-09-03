#include "prette/vao/vao_events.h"
#include "prette/vao/vao.h"

namespace prt::vao {
  VaoId VaoEvent::GetVaoId() const {
    return GetVao()->GetId();
  }

  std::string VaoCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "VaoCreatedEvent(";
    ss << "vao=" << GetVao()->ToString();
    ss << ")";
    return ss.str();
  }

  std::string VaoDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "VaoDestroyedEvent(";
    ss << "vao=" << GetVao()->ToString();
    ss << ")";
    return ss.str();
  }
}