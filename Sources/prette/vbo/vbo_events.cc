#include "prette/vbo/vbo_events.h"

#include <sstream>
#include "prette/vbo/vbo.h"

namespace prt::vbo {
  VboId VboEvent::GetVboId() const {
    return GetVbo()->GetId();
  }

  std::string VboCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "VboDestroyedEvent(";
    ss << "id=" << GetVboId();
    ss << ")";
    return ss.str();
  }

  std::string VboDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "VboDestroyedEvent(";
    ss << "id=" << GetVboId();
    ss << ")";
    return ss.str();
  }
}