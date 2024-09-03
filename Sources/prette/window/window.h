#ifndef PRT_WINDOW_H
#define PRT_WINDOW_H

#include "prette/uuid.h"
#include "prette/shape.h"
#include "prette/input.h"
#include "prette/gui/gui_frame.h"
#include "prette/window/monitor.h"
#include "prette/relaxed_atomic.h"
#include "prette/window/window_events.h"

namespace prt {
  namespace window {
    rx::observable<WindowEvent*> OnWindowEvent();

#define DEFINE_ON_WINDOW_EVENT(Name)                            \
    static inline rx::observable<Name##Event*>                  \
    On##Name##Event() {                                         \
      return OnWindowEvent()                                    \
        .filter(Name##Event::Filter)                            \
        .map(Name##Event::Cast);                                \
    }
    FOR_EACH_WINDOW_EVENT(DEFINE_ON_WINDOW_EVENT)
#undef DEFINE_ON_WINDOW_EVENT

    class Window;
    class WindowVisitor {
    protected:
      WindowVisitor() = default;
    public:
      virtual ~WindowVisitor() = default;
      virtual bool VisitWindow(Window* window) = 0;
    };

    class Window : public WindowEventPublisher {
      friend class WindowBuilder;
#ifdef PRT_GLFW
    public:
      typedef GLFWwindow Handle;
    private:
      template<const int Attribute>
      class WindowAttribute {
      public:
        static inline int
        Get(Handle* handle) {
          return glfwGetWindowAttrib(handle, Attribute);
        }
      };

      template<const int Attribute>
      class EditableWindowAttribute : public WindowAttribute<Attribute> {
      public:
        static inline void
        Set(Handle* handle, const int value) {
          return glfwSetWindowAttrib(handle, Attribute, value);
        }

        template<typename T>
        static inline void
        Set(Handle* handle, const int value) {
          return Set(handle, static_cast<int>(value));
        }
      };

      class DecoratedAttr : public EditableWindowAttribute<GLFW_DECORATED>{};
      class ResizableAttr : public EditableWindowAttribute<GLFW_RESIZABLE>{};
      class FloatingAttr : public EditableWindowAttribute<GLFW_FLOATING>{};
      class AutoIconifyAttr : public EditableWindowAttribute<GLFW_AUTO_ICONIFY>{};
      class FocusOnShowAttr : public EditableWindowAttribute<GLFW_FOCUS_ON_SHOW>{};

      class FocusedAttr : public WindowAttribute<GLFW_FOCUSED>{};
      class IconifiedAttr : public WindowAttribute<GLFW_ICONIFIED>{};
      class MaximizedAttr : public WindowAttribute<GLFW_MAXIMIZED>{};
      class HoveredAttr : public WindowAttribute<GLFW_HOVERED>{};
      class VisibleAttr : public WindowAttribute<GLFW_VISIBLE>{};
      class TransparentFramebufferAttr : public WindowAttribute<GLFW_TRANSPARENT_FRAMEBUFFER>{};

      static inline Window*
      GetWindow(Handle* handle) {
        return (Window*) glfwGetWindowUserPointer(handle);
      }

      static void OnWindowClosed(Handle* handle);
      static void OnWindowPos(Handle* handle, const int xPos, const int yPos);
      static void OnWindowSize(Handle* handle, const int width, const int height);
      static void OnWindowFocus(Handle* handle, int focused);
      static void OnWindowIconify(Handle* handle, int iconified);
      static void OnWindowRefresh(Handle* handle);
      static void OnWindowMaximize(Handle* handle, int maximized);
      static void OnWindowContentScale(Handle* handle, float xScale, float yScale);
#else
#error "Unsupported Platform."
#endif
    protected:
      Handle* handle_;
      UUID id_;
      gui::Frame frame_;
      rx::subscription on_terminating_;
      rx::subscription on_post_init_;
#ifdef PRT_GLFW
      rx::subscription on_post_render_;
#endif //PRT_GLFW

      explicit Window(Handle* handle);
    public:
      ~Window() override;

      inline Handle* GetHandle() const {
        return handle_;
      }

      gui::Frame* GetFrame() { //TODO: reduce visibility
        return &frame_;
      }

      std::string GetTitle() const;
      void SetTitle(const std::string& value);
      Point GetPos() const;
      void SetPos(const Point& value);
      glm::i32vec2 GetSize() const;
      void SetSize(const glm::i32vec2& value);
      void Show();
      void Hide();
      void Close();

      void SetDecorated(const bool value) {
        return DecoratedAttr::Set(GetHandle(), value);
      }

      void SetResizable(const bool value) {
        return ResizableAttr::Set(GetHandle(), value);
      }

      void SetFloating(const bool value) {
        return FloatingAttr::Set(GetHandle(), value);
      }

      void SetAutoIconify(const bool value) {
        return AutoIconifyAttr::Set(GetHandle(), value);
      }
      
      void SetFocusOnShow(const bool value) {
        return FocusOnShowAttr::Set(GetHandle(), value);
      }

      bool IsFocused() const {
        return FocusedAttr::Get(GetHandle());
      }

      bool IsIconified() const {
        return IconifiedAttr::Get(GetHandle());
      }

      bool IsMaximized() const {
        return MaximizedAttr::Get(GetHandle());
      }

      bool IsHovered() const {
        return HoveredAttr::Get(GetHandle());
      }

      bool IsVisible() const {
        return VisibleAttr::Get(GetHandle());
      }

      bool IsResizable() const {
        return ResizableAttr::Get(GetHandle());
      }

      bool IsDecorated() const {
        return DecoratedAttr::Get(GetHandle());
      }

      bool IsAutoIconify() const {
        return AutoIconifyAttr::Get(GetHandle());
      }

      bool IsFloating() const {
        return FloatingAttr::Get(GetHandle());
      }

      bool IsTransparentFramebuffer() const {
        return TransparentFramebufferAttr::Get(GetHandle());
      }

      bool IsFocusOnShow() const {
        return FocusOnShowAttr::Get(GetHandle());
      }

      void SwapBuffers();
      void SetMinSize(const glm::i32vec2& size);

      glm::vec2 GetContentScale() const;

      bool Accept(WindowVisitor* vis) {
        return vis->VisitWindow(this);
      }

      Rectangle GetBounds() const {
        const auto size = GetSize();
        return Rectangle(Point(), size[0], size[1]);
      }
    };

    void InitWindows();
    Window* GetAppWindow();
    bool VisitAllWindows(WindowVisitor* vis);
  }
  using window::Window;
  using window::GetAppWindow;
  using window::VisitAllWindows;
}

#endif //PRT_WINDOW_H