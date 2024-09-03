#include "prette/rbo/rbo_events.h"

#include <sstream>
#include "prette/rbo/rbo.h"

namespace prt::rbo {
  RboId RboEvent::GetRboId() const {
    return GetRbo()->GetId();
  }

  std::string RboCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "RboCreatedEvent(";
    ss << "id=" << GetRboId();
    ss << ")";
    return ss.str();
  }
}