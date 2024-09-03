#include "prette/texture/texture_events.h"
#include <sstream>

namespace prt::texture {
  std::string TextureCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "TextureCreatedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string TextureDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "TextureDestroyedEvent(";
    ss << ")";
    return ss.str();
  }
}