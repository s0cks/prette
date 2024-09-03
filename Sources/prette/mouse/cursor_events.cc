#include "prette/mouse/cursor_events.h"
#include <sstream>
#include "prette/mouse/cursor.h"

namespace prt::mouse {
  std::string CursorCreatedEvent::ToString() const {
    std::stringstream ss;
    ss << "CursorCreatedEvent(";
    ss << "cursor=" << GetCursor()->ToString();
    ss << ")";
    return ss.str(); 
  }

  std::string CursorDeletedEvent::ToString() const {
    std::stringstream ss;
    ss << "CursorDeletedEvent(";
    ss << "cursor=" << GetCursor()->ToString();
    ss << ")";
    return ss.str(); 
  }

  std::string CursorChangedEvent::ToString() const {
    std::stringstream ss;
    ss << "CursorChangedEvent(";
    ss << "cursor=" << GetCursor()->ToString();
    ss << ")";
    return ss.str(); 
  }
}