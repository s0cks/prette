#include <glog/logging.h>
#include <units.h>
#include <uv.h>

#include <backward.hpp>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <termcolor/termcolor.hpp>

#include "prette/engine.h"
#include "prette/os_thread.h"
#include "prette/renderer.h"
#include "prette/signals.h"
#include "prette/window.h"

using namespace prt;

static inline void OnUnhandledException() {
  CrashReport report(CrashReportCause::New(std::current_exception()));
  report.Print();
  LOG(FATAL) << "unhandled exception occured.";
}

auto main(int argc, char** argv) -> int {
  // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
  ::google::InitGoogleLogging(argv[0]);
  ::google::ParseCommandLineFlags(&argc, &argv, true);

  srand(time(nullptr));
  InitSignalHandlers();
  std::set_terminate(OnUnhandledException);
  LOG_IF(FATAL, !SetCurrentThreadName("main")) << "failed to set main thread name.";
  gfx::Init();
  LuaState::Init();
  Engine::Init();
  const auto engine = Engine::Get();
  ASSERT(engine);
  return engine->Run();
}