#include "prette/window/window.h"

#include <unordered_map>
#include "prette/os_thread.h"
#include "prette/thread_local.h"
#include "prette/engine/engine.h"
#include "prette/render/renderer.h"
#include "prette/window/window_flags.h"
#include "prette/window/window_builder.h"

namespace prt::window {
  static ThreadLocal<Window> app_;

  static inline Window*
  CreateAppWindow() {
    WindowBuilder builder("App Demo");
    builder.SetFocusOnShow(true);
    builder.SetVisible(false);
    builder.SetResizable(true);
#ifdef OS_IS_OSX
    builder.SetRetinaFramebuffer(true);
    builder.SetGraphicsSwitching(true);
#endif //OSX
    const auto size = GetWindowSize();
    builder.SetWidth(size.width());
    builder.SetHeight(size.height());
    const auto window = builder.Build();
    if(!window->IsMaximized()) {
      const auto resolution = render::GetTargetResolution();
      window->SetMinSize(glm::i32vec2(resolution.width(), resolution.height()));
    }
    return window;
  }

  static inline void
  SetAppWindow(Window* window) {
    PRT_ASSERT(window);
    app_.Set(window);
  }

  Window* GetAppWindow() {
    return app_.Get();
  }

  void InitWindows() {
    DLOG(INFO) << "initializing Windows....";
    SetAppWindow(CreateAppWindow());
  }

  Window::Window(Handle* handle):
    WindowEventPublisher(),
    handle_(handle),
    id_(),
    frame_(),
    on_terminating_(),
    on_post_init_() {
    PRT_ASSERT(handle);
    const auto size = GetSize();
    frame_.SetBounds(Rectangle(Point(), size[0], size[1]));
    const auto engine = engine::GetEngine();
    PRT_ASSERT(engine);
    on_terminating_ = engine->OnTerminatingEvent()
      .subscribe([this](engine::TerminatingEvent* event) {
        Close();
      });
    on_post_init_ = engine->OnPostInitEvent()
      .subscribe([this](engine::PostInitEvent* event) {
        Show();
      });
#ifdef PRT_GLFW
    glfwSetWindowUserPointer(handle, this);
    on_post_render_ = render::OnPostRenderEvent()
      .subscribe([this](render::PostRenderEvent* event) {
        SwapBuffers();
      });
#else
#error "Unsupported Platform."
#endif //PRT_GLFW
  }

  Window::~Window() {
    on_terminating_.unsubscribe();
    on_post_init_.unsubscribe();
#ifdef PRT_GLFW
    on_post_render_.unsubscribe();
#endif //PRT_GLFW
  }

  rx::observable<WindowEvent*> OnWindowEvent() {
    const auto window = GetAppWindow();
    PRT_ASSERT(window);
    return window->OnEvent();
  }

  bool VisitAllWindows(WindowVisitor* vis) {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return false;
  }
}