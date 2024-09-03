#include "prette/engine/engine_events.h"

#include <sstream>
#include "prette/common.h"

namespace prt::engine {
  std::string PreInitEvent::ToString() const {
    std::stringstream ss;
    ss << "PreInitEvent(";
    ss << "engine=" << engine();
    ss << ")";
    return ss.str();
  }

  std::string PostInitEvent::ToString() const {
    std::stringstream ss;
    ss << "PostInitEvent(";
    ss << "engine=" << engine();
    ss << ")";
    return ss.str();
  }

  std::string PreTickEvent::ToString() const {
    std::stringstream ss;
    ss << "PreTickEvent(";
    ss << "engine=" << engine();
    ss << ")";
    return ss.str();
  }

  std::string TickEvent::ToString() const {
    std::stringstream ss;
    ss << "TickEvent(";
    ss << "engine=" << engine() << ", ";
    ss << "current=" << GetCurrentTick() << ", ";
    ss << "previous=" << GetPreviousTick();
    ss << ")";
    return ss.str();
  }

  std::string PostTickEvent::ToString() const {
    std::stringstream ss;
    ss << "PostTickEvent(";
    ss << "engine=" << engine() << ", ";
    ss << "current=" << tick();
    ss << ")";
    return ss.str();
  }

  std::string TerminatingEvent::ToString() const {
    std::stringstream ss;
    ss << "TerminatingEvent(";
    ss << "engine=" << engine();
    ss << ")";
    return ss.str();
  }

  std::string TerminatedEvent::ToString() const {
    std::stringstream ss;
    ss << "TerminatedEvent(";
    ss << "engine=" << engine();
    ss << ")";
    return ss.str();
  }

  std::string InitializedEvent::ToString() const {
    std::stringstream ss;
    ss << "InitializedEvent(";
    ss << ")";
    return ss.str();
  }
}