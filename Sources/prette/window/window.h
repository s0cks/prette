#ifndef PRT_WINDOW_H
#define PRT_WINDOW_H

#include "prette/uuid.h"
#include "prette/shape.h"
#include "prette/window/monitor.h"
#include "prette/window/window_events.h"

namespace prt {
  namespace window {
    class Window;
    class WindowVisitor {
      DEFINE_NON_COPYABLE_TYPE(WindowVisitor);
    protected:
      WindowVisitor() = default;
    public:
      virtual ~WindowVisitor() = default;
      virtual auto VisitWindow(Window* window) -> bool = 0;
    };

    class Window : public WindowEventSource {
      friend class WindowBuilder;
      DEFINE_NON_COPYABLE_TYPE(Window);
    public:
      struct Comparator {
        auto operator()(Window* lhs, Window* rhs) const -> bool {
          PRT_ASSERT(lhs);
          PRT_ASSERT(rhs);
          return lhs->GetTitle() < rhs->GetTitle(); //TODO: investigate using UUIDs?
        }
      };
#ifdef PRT_GLFW
    public:
      using Handle = GLFWwindow;
    private:
      template<const int Attribute>
      class WindowAttribute {
      public:
        static inline auto
        Get(Handle* handle) -> int {
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

      static inline auto
      GetWindow(Handle* handle) -> Window*{
        return (Window*) glfwGetWindowUserPointer(handle); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
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
    private:
      WindowEventSubject events_;
      Handle* handle_;
      UUID id_;
      rx::subscription on_post_init_{};

      explicit Window(Handle* handle);
      void PublishEvent(WindowEvent* event) override;
    public:
      ~Window() override;

      inline auto GetHandle() const -> Handle* {
        return handle_;
      }

      auto GetTitle() const -> std::string;
      void SetTitle(const std::string& value);
      auto GetPos() const -> Point;
      void SetPos(const Point& value);
      auto GetSize() const -> glm::i32vec2;
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

      auto IsFocused() const -> bool {
        return FocusedAttr::Get(GetHandle());
      }

      auto IsIconified() const -> bool {
        return IconifiedAttr::Get(GetHandle());
      }

      auto IsMaximized() const -> bool {
        return MaximizedAttr::Get(GetHandle());
      }

      auto IsHovered() const -> bool {
        return HoveredAttr::Get(GetHandle());
      }

      auto IsVisible() const -> bool {
        return VisibleAttr::Get(GetHandle());
      }

      auto IsResizable() const -> bool {
        return ResizableAttr::Get(GetHandle());
      }

      auto IsDecorated() const -> bool {
        return DecoratedAttr::Get(GetHandle());
      }

      auto IsAutoIconify() const -> bool {
        return AutoIconifyAttr::Get(GetHandle());
      }

      auto IsFloating() const -> bool {
        return FloatingAttr::Get(GetHandle());
      }

      auto IsTransparentFramebuffer() const -> bool {
        return TransparentFramebufferAttr::Get(GetHandle());
      }

      auto IsFocusOnShow() const -> bool {
        return FocusOnShowAttr::Get(GetHandle());
      }

      void SwapBuffers();
      auto ToString() const -> std::string;
      void SetMinSize(const glm::i32vec2& size);

      auto GetContentScale() const -> glm::vec2;

      auto Accept(WindowVisitor* vis) -> bool {
        return vis->VisitWindow(this);
      }

      auto GetBounds() const -> Rectangle {
        const auto size = GetSize();
        return Rectangle(Point(), size[0], size[1]);
      }

      auto OnEvent() const -> WindowEventObservable override {
        return events_.get_observable();
      }

      friend auto operator<<(std::ostream& stream, Window* rhs) -> std::ostream& {
        return stream << rhs->ToString();
      }
    };

    using WindowSet = std::set<Window*, Window::Comparator>;

    void InitWindows();
    auto GetAppWindow() -> Window*;
    auto GetAllWindows() -> const WindowSet&;
    auto GetTotalNumberOfWindows() -> uword;
    auto VisitAllWindows(const std::function<bool(Window*)>& vis) -> bool;
    auto VisitAllWindows(WindowVisitor* vis) -> bool;
  }
  using window::Window;
  using window::GetAppWindow;
  using window::VisitAllWindows;
}

#endif //PRT_WINDOW_H