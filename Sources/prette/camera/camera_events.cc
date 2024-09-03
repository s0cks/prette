#include "prette/camera/camera_events.h"
#include <sstream>

namespace prt::camera {
  std::string CameraCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "CameraCreatedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string CameraDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "CameraDestroyedEvent(";
    ss << ")";
    return ss.str();
  }
}