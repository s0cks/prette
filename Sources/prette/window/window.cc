#include "prette/window/window.h"

#include <unordered_map>
#include "prette/thread_local.h"
#include "prette/engine/engine.h"
#include "prette/window/window_events.h"
#include "prette/window/window_flags.h"
#include "prette/window/window_builder.h"

namespace prt::window {
  static ThreadLocal<Window> app_; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static inline auto
  CreateAppWindow() -> Window* {
    WindowBuilder builder("App Demo");
    builder.SetFocusOnShow(true);
    builder.SetVisible(false);
    builder.SetResizable(false);
#ifdef OS_IS_OSX
    builder.SetRetinaFramebuffer(true);
    builder.SetGraphicsSwitching(true);
#endif //OSX
    const auto size = GetWindowSize();
    builder.SetWidth(size.width());
    builder.SetHeight(size.height());
    const auto window = builder.Build();
    if(!window->IsMaximized()) {
      //TODO: set Window min-size
      // window->SetMinSize(glm::i32vec2(resolution.width(), resolution.height()));
    }
    return window;
  }

  static inline void
  SetAppWindow(Window* window) {
    PRT_ASSERT(window);
    app_.Set(window);
  }

  auto GetAppWindow() -> Window* {
    return app_.Get();
  }

  void InitWindows() {
    DLOG(INFO) << "initializing Windows....";
    SetAppWindow(CreateAppWindow());
  }

  Window::Window(Handle* handle):
    WindowEventSource(),
    handle_(handle),
    events_(),
    id_(),
    on_terminating_(),
    on_post_init_() {
    PRT_ASSERT(handle);
    const auto size = GetSize();
    const auto engine = engine::GetEngine();
    PRT_ASSERT(engine);
    //TODO: on_terminating_ & on_post_init discard 16 bytes of data?
    on_terminating_ = engine->OnTerminating() // NOLINT(cppcoreguidelines-slicing)
      .subscribe([this](engine::TerminatingEvent* event) {
        Close();
      });
    on_post_init_ = engine->OnPostInit() // NOLINT(cppcoreguidelines-slicing)
      .subscribe([this](engine::PostInitEvent* event) {
        Show();
      });
#ifdef PRT_GLFW
    glfwSetWindowUserPointer(handle, this);
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

  auto OnWindowEvent() -> WindowEventObservable {
    const auto window = GetAppWindow();
    PRT_ASSERT(window);
    return window->OnEvent();
  }

  void Window::PublishEvent(WindowEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

  auto VisitAllWindows(WindowVisitor* vis) -> bool {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return false;
  }

  auto Window::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Window(";
    ss << "title=" << GetTitle() << ", ";
    ss << "size=" << GetSize();
    ss << ")";
    return ss.str();
  }
}