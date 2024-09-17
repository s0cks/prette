#include "prette/mouse/cursor_events.h"
#include <sstream>
#include "prette/mouse/cursor.h"

namespace prt::mouse {
  auto CursorCreatedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "CursorCreatedEvent(";
    ss << "cursor=" << GetCursor()->ToString();
    ss << ")";
    return ss.str();
  }

  auto CursorDeletedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "CursorDeletedEvent(";
    ss << "cursor=" << GetCursor()->ToString();
    ss << ")";
    return ss.str();
  }

  auto CursorChangedEvent::ToString() const -> std::string {
    std::stringstream ss;
    ss << "CursorChangedEvent(";
    ss << "cursor=" << GetCursor()->ToString();
    ss << ")";
    return ss.str();
  }
}