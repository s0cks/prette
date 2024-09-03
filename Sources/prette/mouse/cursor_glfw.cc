#include "prette/mouse/cursor.h"

#include <sstream>
#include "prette/thread_local.h"
#include "prette/window/window.h"
#include "prette/mouse/mouse_flags.h"

namespace prt::mouse {
  std::string GlfwCursor::ToString() const {
    std::stringstream ss;
    ss << "GlfwCursor(";
    ss << "handle=" << GetHandle();
    ss << ")";
    return ss.str();
  }

  Cursor* GlfwCursor::New(const img::Image* img) {
    PRT_ASSERT(img);
    GLFWimage image;
    image.height = img->height();
    image.width = img->width();
    image.pixels = (unsigned char*)img->data();
    const auto handle = glfwCreateCursor(&image, 0, 0);
    return handle ? New(handle) : nullptr;
  }

  Cursor* GlfwCursor::NewStandard(const Shape shape) {
    const auto handle = glfwCreateStandardCursor(shape);
    PRT_ASSERT(handle);
    return New(handle);
  }

  Cursor* GlfwCursor::New(const uri::Uri& uri) {
    if(uri.HasScheme("cursor")) {
      const auto root_dir = GetCursorsDir();
      auto path = uri.path;
      if(!StartsWith(path, root_dir))
        path = fmt::format("{0:s}/{1:s}", root_dir, path[0] == '/' ? path.substr(1) : path);
      return New(uri::Uri(fmt::format("file://{0:s}", path)));
    } else if(!uri.HasScheme("file")) {
      LOG(ERROR) << "invalid Cursor uri: " << uri;
      return nullptr;
    }
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension());
    DLOG(INFO) << "loading Cursor from: " << uri << "....";
    const auto image = img::Decode(uri);
    if(!image) {
      LOG(ERROR) << "failed to decode Cursor image from: " << uri;
      return nullptr;
    }
    return New(image);
  }

  static ThreadLocal<Cursor> default_cursor_;
  static ThreadLocal<Cursor> current_;

  void SetDefaultCursor(Cursor* cursor) { //TODO: probably should free existing default_cursor
    PRT_ASSERT(cursor);
    DLOG_IF(INFO, default_cursor_) << "overwriting default_cursor: " << default_cursor_.Get()->ToString();
    default_cursor_.Set(cursor);
    if(!current_)
      current_.Set(cursor);
  }

  void SetDefaultCursor(const uri::Uri& uri) {
    const auto cursor = GlfwCursor::New(uri);
    DLOG_IF(ERROR, !cursor) << "failed to create GlfwCursor from: " << uri;
    if(cursor)
      SetDefaultCursor(cursor);
  }

  Cursor* GetDefaultCursor() {
    if(default_cursor_)
      return default_cursor_.Get();
    const auto cursor = GlfwCursor::NewStandard(GlfwCursor::kArrow);
    default_cursor_.Set(cursor);
    return cursor;
  }

  void SetCurrentCursor(Cursor* cursor) {
    if(!cursor)
      return SetCurrentCursor(GetDefaultCursor());
    PRT_ASSERT(cursor);
    DLOG_IF(INFO, current_) << "overwriting current_cursor: " << current_.Get()->ToString();
    current_.Set(cursor);
    if(!default_cursor_)
      default_cursor_.Set(cursor);
    const auto window = GetAppWindow();
    PRT_ASSERT(window);
    glfwSetCursor(window->GetHandle(), ((GlfwCursor*)cursor)->GetHandle());
  }

  void SetCurrentCursor(const uri::Uri& uri) {
    const auto cursor = GlfwCursor::New(uri);
    DLOG_IF(ERROR, !cursor) << "failed to create GlfwCursor from: " << uri;
    if(cursor)
      SetCurrentCursor(cursor);
  }

  Cursor* GetCurrentCursor() {
    return current_.Get();
  }
}