#ifndef PRT_WINDOW_BUILDER_H
#define PRT_WINDOW_BUILDER_H

#include "prette/builder.h"
#include "prette/window/window.h"
#include "prette/window/monitor.h"

namespace prt::window {
  class Window;
  class WindowBuilder : public BuilderTemplate<Window> {
    DEFINE_NON_COPYABLE_TYPE(WindowBuilder);
  private:
    std::string title_;
    glm::i32vec2 size_;
    Monitor* monitor_{};
    Window* share_{};
  private:
    auto GetShareHandle() const -> GLFWwindow*;
    auto GetMonitorHandle() const -> GLFWmonitor*;
  public:
    WindowBuilder(const char* title = ""):
      BuilderTemplate<Window>(),
      title_(title),
      size_() {
    }
    ~WindowBuilder() override = default;

    auto HasMonitor() const -> bool {
      return monitor_ != nullptr;
    }

    auto GetMonitor() const -> Monitor* {
      return monitor_;
    }

    void SetSize(const glm::i32vec2& size) {
      size_ = size;
    }

    void SetWidth(const int32_t width) {
      size_[0] = width;
    }

    void SetHeight(const int32_t height) {
      size_[1] = height;
    }

    auto GetSize() const -> const glm::i32vec2 {
      return size_;
    }

    auto HasShare() const -> bool {
      return share_ != nullptr;
    }

    auto GetShare() const -> Window* {
      return share_;
    }

    void SetShare(Window* share) {
      share_ = share;
    }

    void SetFloating(const bool value);
    void SetResizable(const bool value);
    void SetVisible(const bool value);
    void SetDecorated(const bool value);
    void SetFocused(const bool value);
    void SetAutoIconify(const bool value);
    void SetMaximized(const bool value);
    void SetCenterCursor(const bool value);
    void SetTransparentFramebuffer(const bool value);
    void SetFocusOnShow(const bool value);
    void SetScaleToMonitor(const bool value);
#ifdef __APPLE__
    void SetRetinaFramebuffer(const bool value);
    void SetGraphicsSwitching(const bool value);
#endif

    auto Build() const -> Window* override;

    auto BuildAsync() const -> rx::observable<Window*> override {
      return rx::observable<>::create<Window*>([this](rx::subscriber<Window*> s) {
        const auto value = Build();
        if(!value)
          return s.on_error(rx::util::make_error_ptr(std::runtime_error("failed to build Window.")));
        s.on_next(value);
        s.on_completed();
      });
    }
  };
}

#endif //PRT_WINDOW_BUILDER_H