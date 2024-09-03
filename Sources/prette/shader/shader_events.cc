#include <sstream>

#include "prette/shader/shader_events.h"
#include "prette/shader/shader.h"

namespace prt::shader {
  ShaderId ShaderEvent::GetShaderId() const {
    return GetShader()->GetId();
  }

  std::string ShaderCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "ShaderCreatedEvent(";
    ss << "id=" << GetShaderId();
    ss << ")";
    return ss.str();
  }

  std::string ShaderDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "ShaderDestroyedEvent(";
    ss << "id=" << GetShaderId();
    ss << ")";
    return ss.str();
  }

  std::string ShaderCompiledEvent::ToString() const {
    std::stringstream ss;
    ss << "ShaderCompiledEvent(";
    ss << "id=" << GetShaderId();
    ss << ")";
    return ss.str();
  }
}