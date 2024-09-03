#include <cstdlib>
#include <glog/logging.h>

#include <time.h>
#include <cstdlib>
#include <uv.h>
#include <stdexcept>
#include <exception>

#include <units.h>
#include <backward.hpp>

#define NK_GLFW_GL3_IMPLEMENTATION

#include "prette/rx.h"
#include "prette/prette.h"
#include "prette/flags.h"
#include "prette/ibo/ibo.h"
#include "prette/font/font.h"
#include "prette/os_thread.h"
#include "prette/mouse/mouse.h"
#include "prette/entity/entity.h"
#include "prette/window/window.h"
#include "prette/engine/engine.h"
#include "prette/shader/shader.h"
#include "prette/render/renderer.h"
#include "prette/resource/resource.h"
#include "prette/keyboard/keyboard.h"
#include "prette/material/material.h"

#include "prette/class.h"
#include "prette/gfx_debug.h"

#include "prette/gui/gui_window.h"

#include "prette/fbo/fbo.h"
#include "prette/render/render_settings.h"

#include "prette/ibo/ibo_scope.h"
#include "prette/ibo/ibo_builder.h"

#include "prette/vbo/vbo_scope.h"
#include "prette/vao/vao_scope.h"
#include "prette/vbo/vbo_builder.h"

#include "prette/os_thread.h"

#include "prette/signals.h"
#include "prette/mouse/cursor.h"

#include "prette/vbo/vbo_scope.h"

#include "prette/runtime.h"
#include "prette/gui/gui_context.h"

using namespace prt;

template<class Event, const google::LogSeverity Severity = google::INFO>
static inline std::function<void(Event*)>
LogEvent() {
  return [](Event* event) {
    LOG_AT_LEVEL(Severity) << event->ToString();
  };
}

class TestWindow : public gui::Window,
                   public mouse::MouseMoveEventSubscriber {
protected:
  rx::subscription on_key_;
  uword count_;

  void Render(gui::Context* ctx) override {
    ctx->SetColor(GetBackground());
    auto transform = glm::mat4(1.0f);
    if((count_ % 2) == 0)
      transform = glm::rotate(transform, glm::radians((count_ % 4) * 45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ctx->DrawTexturedRect(transform, GetBounds());
  }

  void OnMouseMove(mouse::MouseMoveEvent* event) override {
    const auto& bounds = GetBounds();
    const auto& mpos = event->pos();
    if(bounds.Contains(mpos)) {
      SetBackground(kRed);
    } else {
      SetBackground(kBlack);
    }
  }
public:
  TestWindow(const uword count):
    Window(),
    mouse::MouseMoveEventSubscriber(mouse::GetMouse()),
    count_(count),
    on_key_() {
    SetPos({ count * 50.0f, count * 50.0f });
    SetSize({ 128, 128 });
    SetBackground(kBlack);
    on_key_ = keyboard::OnKeyPressedEvent()
      .filter(keyboard::KeyEvent::FilterBy(GLFW_KEY_E))
      .map(keyboard::KeyEvent::Cast)
      .subscribe([this](keyboard::KeyEvent* event) {
        DLOG(INFO) << event->ToString();
        SetBackground(kGreen);
      });
  }
  ~TestWindow() override = default;
};

static inline void
OnUnhandledException() {
  CrashReportCause cause(std::current_exception());
  CrashReport report(cause);
  report.Print(std::cerr);
}

static inline const char*
GetSeverityName(const google::LogSeverity sev) {
  switch(sev) {
    case google::INFO:
      return "INFO";
    default:
      return "Unknown";
  }
}
// void MyPrefixFormatter(std::ostream& s, const google::LogMessage& m, void* /*data*/) {

//   using namespace std;
//   s << GetSeverityName(m.severity())
//   << setw(4) << 1900 + m.time().year()
//   << setw(2) << 1 + m.time().month()
//   << setw(2) << m.time().day()
//   << ' '
//   << setw(2) << m.time().hour() << ':'
//   << setw(2) << m.time().min()  << ':'
//   << setw(2) << m.time().sec() << "."
//   << setw(6) << m.time().usec()
//   << ' '
//   << setfill(' ') << setw(5)
//   << m.thread_id() << setfill('0')
//   << ' '
//   << m.basename() << ':' << m.line() << "]";
// }

int main(int argc, char** argv) {  
  srand(time(NULL));

  // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
  ::google::InitGoogleLogging(argv[0]);
  ::google::LogToStderr();
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  PrintRuntimeInfo();
  InitSignalHandlers();
  std::set_terminate(OnUnhandledException);
  LOG_IF(FATAL, !SetCurrentThreadName("main")) << "failed to set main thread name.";
  
  gfx::Init();
  engine::InitEngine();
  window::InitWindows();
  mouse::InitMouse();
  keyboard::InitKeyboard();
  render::InitRenderer();
  // init classes
  Class::Init();
  gui::Vertex::InitClass();

  uword counter = 0;
  keyboard::OnKeyPressedEvent()
    .subscribe(LogEvent<keyboard::KeyPressedEvent>());
  keyboard::OnKeyPressedEvent()
    .filter(keyboard::KeyPressedEvent::FilterBy(GLFW_KEY_SPACE))
    .subscribe([&counter](keyboard::KeyPressedEvent* e) {
      DLOG(INFO) << "opening gui...";
      new TestWindow(counter++);
    });
  keyboard::OnKeyPressedEvent()
    .filter(keyboard::KeyPressedEvent::FilterBy(GLFW_KEY_GRAVE_ACCENT))
    .subscribe([](keyboard::KeyPressedEvent* e) {
      PrintRuntimeInfo();
    });

  const auto engine = engine::GetEngine();
  PRT_ASSERT(engine);
  engine->Run();
  return EXIT_SUCCESS;
}