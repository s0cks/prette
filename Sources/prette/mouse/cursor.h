#ifndef PRT_CURSOR_H
#define PRT_CURSOR_H

#include <string>
#include <glog/logging.h>

#include "prette/rx.h"
#include "prette/uri.h"
#include "prette/mouse/cursor_events.h"

namespace prt::mouse {
  auto OnCursorEvent() -> rx::observable<CursorEvent*>;
#define DEFINE_ON_CURSOR_EVENT(Name)                    \
  static inline auto                                    \
  On##Name##Event() -> rx::observable<Name##Event*> {   \
    return OnCursorEvent()                              \
      .filter(Name##Event::Filter)                      \
      .map(Name##Event::Cast);                          \
  }
  FOR_EACH_CURSOR_EVENT(DEFINE_ON_CURSOR_EVENT)
#undef DEFINE_ON_CURSOR_EVENT

  class Cursor;
  class CursorVisitor {
  protected:
    CursorVisitor() = default;
  public:
    virtual ~CursorVisitor() = default;
    virtual auto VisitCursor(Cursor* cursor) -> bool = 0;
  };

  class Cursor {
  protected:
    Cursor() = default;

    static void Publish(CursorEvent* event);

    template<class E, typename... Args>
    static inline void
    Publish(Args... args) {
      E event(args...);
      return Publish((CursorEvent*) &event);
    }
  public:
    virtual ~Cursor() = default;

    auto Accept(CursorVisitor* vis) -> bool {
      PRT_ASSERT(vis);
      return vis->VisitCursor(this);
    }

    virtual auto ToString() const -> std::string = 0;
  };

  auto ListAvailableCursors() -> rx::observable<std::string>;

  void SetDefaultCursor(Cursor* cursor);
  void SetDefaultCursor(const uri::Uri& uri);
  auto GetDefaultCursor() -> Cursor*;

  static inline void
  SetDefaultCursor(const uri::basic_uri& uri) {
    if(!(StartsWith(uri, "file:") && EndsWith(uri, ".png"))
    && !StartsWith(uri, "cursor:")) {
      LOG(WARNING) << "invalid Cursor Uri: " << uri;
      return SetDefaultCursor(uri::Uri(fmt::format("cursor://{0:s}", uri)));
    }
    return SetDefaultCursor(uri::Uri(uri));
  }

  void SetCurrentCursor(Cursor* cursor);
  void SetCurrentCursor(const uri::Uri& uri);
  auto GetCurrentCursor() -> Cursor*;

  static inline void
  SetCurrentCursor(const uri::basic_uri& uri) {
    if(!(StartsWith(uri, "file:") && EndsWith(uri, ".png"))
    && !StartsWith(uri, "cursor:")) {
      LOG(WARNING) << "invalid Cursor Uri: " << uri;
      return SetCurrentCursor(uri::Uri(fmt::format("cursor://{0:s}", uri)));
    }
    return SetCurrentCursor(uri::Uri(uri));
  }
}

#endif //PRT_CURSOR_H