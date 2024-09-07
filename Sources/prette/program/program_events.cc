#include "prette/program/program_events.h"

#include <sstream>

namespace prt::program {
  auto ProgramCreatedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ProgramCreatedEvent(";
    ss << "id=" << GetProgramId();
    ss << ")";
    return ss.str();
  }

  auto ProgramShaderAttachedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ProgramShaderAttachedEvent(";
    ss << "id=" << GetProgramId() << ", ";
    ss << "shader=" << GetShaderId();
    ss << ")";
    return ss.str();
  }

  auto ProgramShaderDetachedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ProgramShaderDetachedEvent(";
    ss << "id=" << GetProgramId() << ", ";
    ss << "shader=" << GetShaderId();
    ss << ")";
    return ss.str();
  }

  auto ProgramLinkedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ProgramLinkedEvent(";
    ss << "id=" << GetProgramId();
    ss << ")";
    return ss.str();
  }

  auto ProgramDestroyedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "ProgramDestroyedEvent(";
    ss << "id=" << GetProgramId();
    ss << ")";
    return ss.str();
  }
}