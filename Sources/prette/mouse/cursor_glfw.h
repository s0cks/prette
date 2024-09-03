#ifndef PRT_CURSOR_H
#error "Please #include <prette/mouse/cursor.h> instead."
#endif //PRT_CURSOR_H

#ifndef PRT_CURSOR_GLFW_H
#define PRT_CURSOR_GLFW_H

#include "prette/gfx.h"
#include "prette/image/image.h"
#include "prette/mouse/cursor.h"

namespace prt::mouse {
  typedef GLFWcursor CursorHandle;

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
  protected:
    CursorHandle* handle_;

    explicit GlfwCursor(CursorHandle* handle):
      Cursor(),
      handle_(handle) {
      PRT_ASSERT(handle);
    }
  public:
    ~GlfwCursor() override = default;
    std::string ToString() const override;

    inline CursorHandle* GetHandle() const { //TODO: reduce visibility
      return handle_;
    }
  private:
    static inline GlfwCursor*
    New(CursorHandle* handle) {
      PRT_ASSERT(handle);
      return new GlfwCursor(handle);
    }
  public:
    static Cursor* NewStandard(const Shape shape = kDefaultShape);
    static Cursor* New(const img::Image* image);
    static Cursor* New(const uri::Uri& uri);

    static inline Cursor*
    New(const uri::basic_uri& uri) {
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