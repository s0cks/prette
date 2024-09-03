#include "prette/component/component_events.h"

#include <sstream>
#include "prette/component/component.h"

namespace prt::component {
  std::string ComponentRegisteredEvent::ToString() const {
    std::stringstream ss;
    ss << "ComponentRegisteredEvent(";
    ss << "component=" << component()->GetName();
    ss << ")";
    return ss.str();
  }

  std::string ComponentStateCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "ComponentStateCreatedEvent(";
    ss << "component=" << component()->GetName();
    ss << ")";
    return ss.str();
  }

  std::string ComponentStateDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "ComponentStateDestroyedEvent(";
    ss << "component=" << component()->GetName();
    ss << ")";
    return ss.str();
  }
}