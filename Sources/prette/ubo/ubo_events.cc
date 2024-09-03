#include "prette/ubo/ubo_events.h"

#include <sstream>
#include "prette/ubo/ubo.h"

namespace prt::ubo {
  UboId UboEvent::GetUboId() const {
    return GetUbo()->GetId();
  }

  std::string UboCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "UboCreatedEvent(";
    ss << "id=" << GetUboId();
    ss << ")";
    return ss.str();
  }

  std::string UboDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "UboDestroyedEvent(";
    ss << "id=" << GetUboId();
    ss << ")";
    return ss.str();
  }
}