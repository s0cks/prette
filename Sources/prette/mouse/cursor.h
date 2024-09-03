#ifndef PRT_CURSOR_H
#define PRT_CURSOR_H

#include <string>
#include <glog/logging.h>

#include "prette/rx.h"
#include "prette/uri.h"
#include "prette/mouse/cursor_events.h"

namespace prt::mouse {
  rx::observable<CursorEvent*> OnCursorEvent();
#define DEFINE_ON_CURSOR_EVENT(Name)            \
  static inline rx::observable<Name##Event*>    \
  On##Name##Event() {                           \
    return OnCursorEvent()                      \
      .filter(Name##Event::Filter)              \
      .map(Name##Event::Cast);                  \
  }
  FOR_EACH_CURSOR_EVENT(DEFINE_ON_CURSOR_EVENT)
#undef DEFINE_ON_CURSOR_EVENT

  class Cursor;
  class CursorVisitor {
  protected:
    CursorVisitor() = default;
  public:
    virtual ~CursorVisitor() = default;
    virtual bool VisitCursor(Cursor* cursor) = 0;
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
    
    bool Accept(CursorVisitor* vis) {
      PRT_ASSERT(vis);
      return vis->VisitCursor(this);
    }

    virtual std::string ToString() const = 0;
  };

  rx::observable<std::string> ListAvailableCursors();

  void SetDefaultCursor(Cursor* cursor);
  void SetDefaultCursor(const uri::Uri& uri);
  Cursor* GetDefaultCursor();

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
  Cursor* GetCurrentCursor();

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

#include "prette/mouse/cursor_glfw.h"

#endif //PRT_CURSOR_H