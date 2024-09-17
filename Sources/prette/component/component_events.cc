#include "prette/component/component_events.h"

#include <sstream>
#include "prette/component/component.h"

namespace prt::component {
  auto ComponentRegisteredEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ComponentRegisteredEvent(";
    ss << "component=" << component()->GetName();
    ss << ")";
    return ss.str();
  }

  auto ComponentStateCreatedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ComponentStateCreatedEvent(";
    ss << "component=" << component()->GetName();
    ss << ")";
    return ss.str();
  }

  auto ComponentStateDestroyedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ComponentStateDestroyedEvent(";
    ss << "component=" << component()->GetName();
    ss << ")";
    return ss.str();
  }
}