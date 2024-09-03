#include "prette/ibo/ibo_events.h"

#include <sstream>
#include "prette/ibo/ibo.h"

namespace prt::ibo {
  IboId IboEvent::GetIboId() const {
    return GetIbo()->GetId();
  }

  std::string IboCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "IboCreatedEvent(";
    ss << "id=" << GetIboId();
    ss << ")";
    return ss.str();
  }

  std::string IboDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "IboDestroyedEvent(";
    ss << "id=" << GetIboId();
    ss << ")";
    return ss.str();
  }
}