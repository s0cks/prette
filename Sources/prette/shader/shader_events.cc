#include <sstream>

#include "prette/shader/shader_events.h"
#include "prette/shader/shader.h"

namespace prt::shader {
  auto ShaderEvent::GetShaderId() const -> ShaderId {
    return GetShader()->GetId();
  }

  auto ShaderCreatedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ShaderCreatedEvent(";
    ss << "shader=" << GetShader()->ToString();
    ss << ")";
    return ss.str();
  }

  auto ShaderDestroyedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ShaderDestroyedEvent(";
    ss << "shader=" << GetShader()->ToString();
    ss << ")";
    return ss.str();
  }

  auto ShaderCompiledEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ShaderCompiledEvent(";
    ss << "shader=" << GetShader()->ToString();
    ss << ")";
    return ss.str();
  }
}