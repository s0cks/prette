#ifndef PRT_CURSOR_GLFW_H
#define PRT_CURSOR_GLFW_H

#include "prette/gfx.h"
#include "prette/image/image.h"
#include "prette/mouse/cursor.h"

namespace prt::mouse {
  using CursorHandle = GLFWcursor;

  class GlfwCursor : public Cursor {
    friend class Cursor;
  public:
    enum Shape : int {
      kArrow        = GLFW_ARROW_CURSOR,
      kIBeam        = GLFW_IBEAM_CURSOR,
      kCrosshair    = GLFW_CROSSHAIR_CURSOR,
      kHand         = GLFW_HAND_CURSOR,
      kHResize      = GLFW_HRESIZE_CURSOR,
      kVResize      = GLFW_VRESIZE_CURSOR,
      kTotalNumberOfShapes,
      kDefaultShape = kArrow,
    };
  private:
    CursorHandle* handle_;
  protected:
    explicit GlfwCursor(CursorHandle* handle):
      Cursor(),
      handle_(handle) {
      PRT_ASSERT(handle);
    }
  public:
    ~GlfwCursor() override = default;
    auto ToString() const -> std::string override;

    inline auto GetHandle() const -> CursorHandle* { //TODO: reduce visibility
      return handle_;
    }
  private:
    static inline auto
    New(CursorHandle* handle) -> GlfwCursor* {
      PRT_ASSERT(handle);
      return new GlfwCursor(handle);
    }
  public:
    static auto NewStandard(const Shape shape = kDefaultShape) -> Cursor*;
    static auto New(const img::Image* image) -> Cursor*;
    static auto New(const uri::Uri& uri) -> Cursor*;

    static inline auto
    New(const uri::basic_uri& uri) -> Cursor* {
      if(!(StartsWith(uri, "file:") && EndsWith(uri, ".png"))
      && !StartsWith(uri, "cursor:")) {
        LOG(WARNING) << "invalid Cursor Uri: " << uri;
        return New(uri::Uri(fmt::format("cursor://{0:s}", uri)));
      }
      return New(uri::Uri(uri));
    }
  };
}

#endif //PRT_CURSOR_GLFW_H