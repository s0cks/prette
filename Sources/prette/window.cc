#include "prette/window.h"

#include <unordered_map>

#include "prette/engine.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"

namespace prt {
DEFINE_string(window_size, kDefaultWindowSize, "The size of the window.");

auto WindowOpenedEvent::ToString() const -> std::string {
  ToStringHelper<WindowOpenedEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowClosedEvent::ToString() const -> std::string {
  ToStringHelper<WindowClosedEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowPosEvent::ToString() const -> std::string {
  ToStringHelper<WindowPosEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowSizeEvent::ToString() const -> std::string {
  ToStringHelper<WindowSizeEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowFocusEvent::ToString() const -> std::string {
  ToStringHelper<WindowFocusEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowIconifyEvent::ToString() const -> std::string {
  ToStringHelper<WindowIconifyEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowRefreshEvent::ToString() const -> std::string {
  ToStringHelper<WindowRefreshEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowMaximizeEvent::ToString() const -> std::string {
  ToStringHelper<WindowMaximizeEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

auto WindowContentScaleEvent::ToString() const -> std::string {
  ToStringHelper<WindowContentScaleEvent> helper;
  helper.AddFieldPtr("window", GetWindow());
  return helper;
}

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static WindowSet all_;
static ThreadLocal<Window> app_;
static rx::subscription on_terminating_;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables))

static inline void Register(Window* window) {
  ASSERT(window);
  const auto& [iter, success] = all_.insert(window);
  LOG_IF(ERROR, !success) << "failed to register: " << window;
}

static inline void Deregister(Window* window) {
  ASSERT(window);
  const auto removed = all_.erase(window);
  LOG_IF(ERROR, removed != 1) << "failed to deregsiter: " << window;
}

Window::Window(Handle* handle) :
  WindowEventSource(),
  handle_(handle),
  events_(),
  id_() {
  Register(this);
  ASSERT(handle);
  const auto engine = engine::GetEngine();
  ASSERT(engine);
  // TODO: on_post_init discard 16 bytes of data?
  on_post_init_ = engine->OnPostInit().subscribe([this](engine::PostInitEvent* event) {
    Show();
  });
#ifdef PRT_GLFW
  glfwSetWindowUserPointer(handle, this);
#else
#error "Unsupported Platform."
#endif  // PRT_GLFW
}

Window::~Window() {
  Deregister(this);
  on_post_init_.unsubscribe();
}

auto OnWindowEvent() -> WindowEventObservable {
  const auto window = GetAppWindow();
  ASSERT(window);
  return window->OnEvent();
}

void Window::PublishEvent(WindowEvent* event) const {
  ASSERT(event);
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

static inline auto CreateAppWindow() -> Window* {
  WindowBuilder builder("App Demo");
  builder.SetFocusOnShow(true);
  builder.SetVisible(false);
  builder.SetResizable(false);
#ifdef OS_IS_OSX
  builder.SetRetinaFramebuffer(true);
  builder.SetGraphicsSwitching(true);
#endif  // OSX
  const auto size = GetWindowSize();
  builder.SetWidth(size.width());
  builder.SetHeight(size.height());
  const auto window = builder.Build();
  ASSERT(window);
  if (!window->IsMaximized()) {
    // TODO: set Window min-size
    //  window->SetMinSize(glm::i32vec2(resolution.width(), resolution.height()));
  }
#ifdef PRT_DEBUG
  window->OnEvent().subscribe([](WindowEvent* event) {
    DLOG(INFO) << "event: " << event->ToString();
  });
#endif  // PRT_DEBUG
  return window;
}

static inline void SetAppWindow(Window* window) {
  ASSERT(window);
  app_.Set(window);
}

auto GetAppWindow() -> Window* {
  return app_.Get();
}

auto GetAllWindows() -> const WindowSet& {
  return all_;
}

auto GetTotalNumberOfWindows() -> uword {
  return all_.size();
}

void InitWindows() {
  DLOG(INFO) << "initializing Windows....";
  const auto window = CreateAppWindow();
  ASSERT(window);
  SetAppWindow(window);
  const auto engine = GetEngine();
  ASSERT(engine);
  // TODO: create close all windows
}
}  // namespace prt