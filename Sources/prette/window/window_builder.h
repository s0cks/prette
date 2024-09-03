#ifndef PRT_WINDOW_BUILDER_H
#define PRT_WINDOW_BUILDER_H

#include "prette/builder.h"
#include "prette/window/window.h"
#include "prette/window/monitor.h"

namespace prt::window {
  class Window;
  class WindowBuilder : public BuilderTemplate<Window> {
  protected:
    std::string title_;
    glm::i32vec2 size_;
    Monitor* monitor_;
    Window* share_;
  private:
    GLFWwindow* GetShareHandle() const;
    GLFWmonitor* GetMonitorHandle() const;
  public:
    WindowBuilder(const std::string& title = ""):
      BuilderTemplate<Window>(),
      title_(title),
      size_(),
      monitor_(nullptr),
      share_(nullptr) {
    }
    virtual ~WindowBuilder() = default;

    bool HasMonitor() const {
      return monitor_ != nullptr;
    }

    Monitor* GetMonitor() const {
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

    const glm::i32vec2 GetSize() const {
      return size_;
    }

    bool HasShare() const {
      return share_ != nullptr;
    }

    Window* GetShare() const {
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

    Window* Build() const override;

    rx::observable<Window*> BuildAsync() const override {
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