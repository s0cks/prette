#include "prette/mouse/cursor.h"

#include "prette/thread_local.h"
#include "prette/file_resolver.h"
#include "prette/mouse/mouse_flags.h"

#include "prette/image/image.h"

namespace prt::mouse {
  static rx::subject<CursorEvent*> events_;

  rx::observable<CursorEvent*> OnCursorEvent() {
    return events_.get_observable();
  }

  void Cursor::Publish(CursorEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  rx::observable<std::string> ListAvailableCursors() {
    return rx::observable<>::create<std::string>([](rx::subscriber<std::string> s) {
      s.on_next("default");
      const auto dir = GetCursorsDir();
      fs::ListFilesInDirectory<true>(dir)
        .filter(fs::FilterByExtension(img::GetValidExtensions()))
        .map([&dir](const fs::directory_entry& entry) {
          const auto& path = (const std::string&) entry.path();
          if(StartsWith(path, dir))
            return path.substr(dir.length() + 1);
          return path;
        })
        .as_blocking()
        .subscribe([&s](const std::string& next) {
          s.on_next(next);
        });
      s.on_completed();
    });
  }
}