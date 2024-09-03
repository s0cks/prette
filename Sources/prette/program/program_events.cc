#include "prette/program/program_events.h"

#include <sstream>

namespace prt::program {
  std::string ProgramCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "ProgramCreatedEvent(";
    ss << "id=" << GetProgramId();
    ss << ")";
    return ss.str();
  }

  std::string ProgramShaderAttachedEvent::ToString() const {
    std::stringstream ss;
    ss << "ProgramShaderAttachedEvent(";
    ss << "id=" << GetProgramId() << ", ";
    ss << "shader=" << GetShaderId();
    ss << ")";
    return ss.str();
  }

  std::string ProgramShaderDetachedEvent::ToString() const {
    std::stringstream ss;
    ss << "ProgramShaderDetachedEvent(";
    ss << "id=" << GetProgramId() << ", ";
    ss << "shader=" << GetShaderId();
    ss << ")";
    return ss.str();
  }

  std::string ProgramLinkedEvent::ToString() const {
    std::stringstream ss;
    ss << "ProgramLinkedEvent(";
    ss << "id=" << GetProgramId();
    ss << ")";
    return ss.str();
  }

  std::string ProgramDestroyedEvent::ToString() const {
    std::stringstream ss;
    ss << "ProgramDestroyedEvent(";
    ss << "id=" << GetProgramId();
    ss << ")";
    return ss.str();
  }
}