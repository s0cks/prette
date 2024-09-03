#include "prette/render/render_events.h"

#include <sstream>

namespace prt::render {
  std::string RendererInitializedEvent::ToString() const {
    std::stringstream ss;
    ss << "RendererInitializedEvent(";
    ss << ")";
    return ss.str();
  }

  std::string PreRenderEvent::ToString() const {
    std::stringstream ss;
    ss << "PreRenderEvent(";
    ss << ")";
    return ss.str();
  }

  std::string PostRenderEvent::ToString() const {
    std::stringstream ss;
    ss << "PostRenderEvent(";
    ss << "tick=" << tick();
    ss << ")";
    return ss.str();
  }

  std::string RenderModeChangedEvent::ToString() const {
    std::stringstream ss;
    ss << "RenderModeChangedEvent(";
    ss << ")";
    return ss.str();
  }
}