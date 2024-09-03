#include "prette/fbo/fbo_events.h"

#include <sstream>
#include "prette/fbo/fbo.h"

namespace prt::fbo {
  FboId FboEvent::GetFboId() const {
    return GetFbo()->GetId();
  }

  std::string FboCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "FboCreatedEvent(";
    ss << "id=" << GetFboId();
    ss << ")";
    return ss.str();
  }

  std::string FboDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "FboDestroyedEvent(";
    ss << "id=" << GetFboId();
    ss << ")";
    return ss.str();
  }
}