#include "prette/window.h"

#include <lua.h>

#include <cstdlib>
#include <unordered_map>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/keyboard.h"
#include "prette/lua.h"
#include "prette/mouse.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"

namespace prt {
static WindowSet all_{};
static WindowEventSubject all_events_{};
static ThreadLocal<Window> app_{};

static inline void PublishWindowEvent(const WindowEventSubject& subject, WindowEvent* event) {
  ASSERT(event);
  const auto& subscriber = subject.get_subscriber();
  return subscriber.on_next(event);
}

DEFINE_string(window_size, kDefaultWindowSize, "The size of the window.");

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
  events_() {
  ASSERT(handle);
  Register(this);
#ifdef PRT_GLFW
  glfwSetWindowUserPointer(handle, this);
#else
#error "Unsupported Platform."
#endif  // PRT_GLFW
  OnPostInitEvent().subscribe([this](PostInitEvent* event) {
    ASSERT(event);
    Show();
  });
  OnTerminatingEvent().subscribe([this](TerminatingEvent* event) {
    ASSERT(event);
    Close();
  });
}

Window::~Window() {
  Deregister(this);
}

auto OnWindowEvent() -> WindowEventObservable {
  return all_events_.get_observable();
}

void Window::PublishEvent(WindowEvent* event) const {
  ASSERT(event);
  PublishWindowEvent(events_, event);
  PublishWindowEvent(all_events_, event);
}

auto Window::ToString() const -> std::string {
  std::stringstream ss;
  ss << "Window(";
  ss << "id=" << GetId() << ", ";
  ss << "title=" << GetTitle() << ", ";
  ss << "size=" << GetSize();
  ss << ")";
  return ss.str();
}

static inline auto CreateAppWindow() -> Window* {
  WindowBuilder builder("App Demo");
  builder.SetFocusOnShow(true);
  builder.SetVisible(false);
  builder.SetResizable(true);
#ifdef OS_IS_OSX
  builder.SetRetinaFramebuffer(true);
  builder.SetGraphicsSwitching(true);
#endif  // OSX
  builder.SetSize(GetWindowSize());
  return builder.Build();
}

auto Window::New(Handle* handle) -> Window* {
  ASSERT(handle);
  const auto window = new Window(handle);
  ASSERT(window);
  window->PublishWindowCreatedEvent();
  return window;
}

static inline void SetAppWindow(Window* window) {
  ASSERT(window);
  app_.Set(window);
}

auto GetAppWindow() -> Window* {
  return app_.Get();
}

auto GetTotalNumberOfWindows() -> uword {
  return all_.size();
}

void Window::SetTable(lua_State* L, const int index) const {
  ASSERT(L);
  lua_pushstring(L, GetTitle().c_str());
  lua_setfield(L, index + -1, "title");

  lua_newtable(L);
  const auto size = GetSize();
  size.SetTable(L, -1);
  lua_setfield(L, index + -1, "size");
}

auto VisitAllWindows(const std::function<bool(Window*)>& vis) -> bool {
  for (const auto& window : all_) {
    ASSERT(window);
    if (!vis(window))
      return false;
  }
  return true;
}

void Window::Init() {
  const auto window = CreateAppWindow();
  ASSERT(window);
  Keyboard::Init(window);
  Mouse::Init(window);
  OnKeyPressed(GLFW_KEY_ESCAPE).subscribe([](KeyStateEvent* event) {
    ASSERT(event);
    const auto engine = Engine::Get();
    ASSERT(engine);
    engine->Shutdown();
  });
  SetAppWindow(window);
}
}  // namespace prt