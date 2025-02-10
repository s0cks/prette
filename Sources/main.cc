#include <glog/logging.h>
#include <units.h>
#include <uv.h>

#include <backward.hpp>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <termcolor/termcolor.hpp>

#include "prette/engine.h"

using namespace prt;

template <class Event, const google::LogSeverity Severity = google::INFO>
static inline auto LogEvent() -> std::function<void(Event*)> {
  return [](Event* event) {
    LOG_AT_LEVEL(Severity) << event->ToString();
  };
}

auto main(int argc, char** argv) -> int {
  // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
  ::google::InitGoogleLogging(argv[0]);
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  LuaState::Init();
  Engine::Init();
  const auto engine = Engine::Get();
  ASSERT(engine);
  return engine->Run();
}