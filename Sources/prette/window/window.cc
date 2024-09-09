#include "prette/window/window.h"

#include <unordered_map>
#include "prette/thread_local.h"
#include "prette/engine/engine.h"
#include "prette/window/window_events.h"
#include "prette/window/window_flags.h"
#include "prette/window/window_builder.h"

namespace prt::window {
  // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
  static WindowSet all_;
  static ThreadLocal<Window> app_;
  static rx::subscription on_terminating_;
  // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables))

  static inline void
  Register(Window* window) {
    PRT_ASSERT(window);
    const auto& [iter,success] = all_.insert(window);
    LOG_IF(ERROR, !success) << "failed to register: " << window;
  }

  static inline void
  Deregister(Window* window) {
    PRT_ASSERT(window);
    const auto removed = all_.erase(window);
    LOG_IF(ERROR, removed != 1) << "failed to deregsiter: " << window;
  }

  Window::Window(Handle* handle):
    WindowEventSource(),
    handle_(handle),
    events_(),
    id_() {
    Register(this);
    PRT_ASSERT(handle);
    const auto size = GetSize();
    const auto engine = engine::GetEngine();
    PRT_ASSERT(engine);
    //TODO: on_post_init discard 16 bytes of data?
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
    Deregister(this);
    on_post_init_.unsubscribe();
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

  auto Window::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Window(";
    ss << "title=" << GetTitle() << ", ";
    ss << "size=" << GetSize();
    ss << ")";
    return ss.str();
  }

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

  auto GetAllWindows() -> const WindowSet& {
    return all_;
  }

  auto VisitAllWindows(WindowVisitor* vis) -> bool {
    PRT_ASSERT(vis);
    for(const auto& window : all_) {
      if(!vis->VisitWindow(window))
        return false;
    }
    return true;
  }

  auto VisitAllWindows(const std::function<bool(Window*)>& vis) -> bool {
    PRT_ASSERT(vis);
    for(const auto& window : all_) {
      if(!vis(window))
        return false;
    }
    return true;
  }

  auto GetTotalNumberOfWindows() -> uword {
    return all_.size();
  }

  static inline void
  CloseAllWindows() {
    const auto close = [](Window* window) {
      PRT_ASSERT(window);
      window->Close();
      return true;
    };
    LOG_IF(ERROR, !VisitAllWindows(close)) << "failed to close all Windows.";
  }

  void InitWindows() {
    DLOG(INFO) << "initializing Windows....";
    SetAppWindow(CreateAppWindow());
    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    on_terminating_ = engine->OnTerminating() // NOLINT(cppcoreguidelines-slicing)
      .subscribe([](engine::TerminatingEvent* event) {
        return CloseAllWindows();
      });
  }
}