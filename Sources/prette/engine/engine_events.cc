#include "prette/engine/engine_events.h"

#include <sstream>
#include "prette/engine/engine.h"

namespace prt::engine {
  auto PreInitEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "PreInitEvent(";
    ss << "engine=" << GetEngine();
    ss << ")";
    return ss.str();
  }

  auto PostInitEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "PostInitEvent(";
    ss << "engine=" << GetEngine();
    ss << ")";
    return ss.str();
  }

  auto PreTickEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "PreTickEvent(";
    ss << "engine=" << GetEngine();
    ss << ")";
    return ss.str();
  }

  auto TickEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "TickEvent(";
    ss << "engine=" << GetEngine() << ", ";
    ss << "current=" << GetCurrentTick() << ", ";
    ss << "previous=" << GetPreviousTick();
    ss << ")";
    return ss.str();
  }

  auto PostTickEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "PostTickEvent(";
    ss << "engine=" << GetEngine() << ", ";
    ss << "current=" << GetTick();
    ss << ")";
    return ss.str();
  }

  auto TerminatingEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "TerminatingEvent(";
    ss << "engine=" << GetEngine();
    ss << ")";
    return ss.str();
  }

  auto TerminatedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "TerminatedEvent(";
    ss << "engine=" << GetEngine();
    ss << ")";
    return ss.str();
  }

  auto InitializedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "InitializedEvent(";
    ss << ")";
    return ss.str();
  }
}