#include <glog/logging.h>
#include <units.h>
#include <uv.h>

#include <backward.hpp>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <termcolor/termcolor.hpp>

#include "prette/runtime.h"

using namespace prt;

template <class Event, const google::LogSeverity Severity = google::INFO>
static inline auto LogEvent() -> std::function<void(Event*)> {
  return [](Event* event) {
    LOG_AT_LEVEL(Severity) << event->ToString();
  };
}

auto main(int argc, char** argv) -> int {
  Runtime::Init(argc, argv);
  return Runtime::Run();
}