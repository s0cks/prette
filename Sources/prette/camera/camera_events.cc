#include "prette/camera/camera_events.h"
#include <sstream>

namespace prt::camera {
  auto CameraCreatedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "CameraCreatedEvent(";
    ss << ")";
    return ss.str();
  }

  auto CameraDestroyedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "CameraDestroyedEvent(";
    ss << ")";
    return ss.str();
  }
}