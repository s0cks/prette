#include "prette/runtime.h"

#include "prette/prette.h"
#include "prette/mouse/mouse.h"
#include "prette/engine/engine.h"

#include "prette/os_thread.h"
#include "prette/window/window.h"
#include "prette/render/renderer.h"
#include "prette/keyboard/keyboard.h"

#include "prette/thread_local.h"
#include "prette/runtime_info_printer.h"

namespace prt {
#ifdef PRT_DEBUG
  void PrintRuntimeInfo(const google::LogSeverity s, const char* file, const int line, const int indent) {
    RuntimeInfoPrinter printer(s, file, line, indent);
    return printer.Print();
  }
#endif //PRT_DEBUG

  void Runtime::OnUnhandledException() {
    CrashReportCause cause(std::current_exception());
    CrashReport report(cause);
    report.Print();
    LOG(FATAL) << "unhandled exception occured.";
  }

  void Runtime::Init(int argc, char** argv) {
    srand(time(NULL));

    // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
    ::google::InitGoogleLogging(argv[0]);
    ::google::ParseCommandLineFlags(&argc, &argv, true);
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
  }

  static ThreadLocal<Query> kTimeQuery;

  Query* Runtime::GetTimeQuery() {
    if(kTimeQuery)
      return kTimeQuery.Get();
    const auto query = Query::NewTimeElapsed();
    kTimeQuery.Set(query);
    return query;
  }

  int Runtime::Run() {
#ifdef PRT_DEBUG
    PrintRuntimeInfo();
#endif //PRT_DEBUG
    const auto engine = GetEngine();
    PRT_ASSERT(engine);
    engine->Run();
    return EXIT_SUCCESS;
  }
}