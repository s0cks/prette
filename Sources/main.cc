#include <cstdlib>
#include <glog/logging.h>

#include <uv.h>
#include <cstdlib>
#include <stdexcept>
#include <exception>

#include <units.h>
#include <backward.hpp>
#include <termcolor/termcolor.hpp>

#define NK_GLFW_GL3_IMPLEMENTATION

#include "prette/runtime.h"
#include "prette/keyboard/keyboard.h"

using namespace prt;

template<class Event, const google::LogSeverity Severity = google::INFO>
static inline auto
LogEvent() -> std::function<void(Event*)> {
  return [](Event* event) {
    LOG_AT_LEVEL(Severity) << event->ToString();
  };
}

// static inline std::ostream& 
// operator<<(std::ostream& s, const google::LogSeverity& severity) {
//   switch(severity) {
//     case google::INFO:
//       return s << termcolor::white << "I";
//     case google::WARNING:
//       return s << termcolor::yellow << "W";
//     case google::ERROR:
//       return s << termcolor::red << "E";
//     case google::FATAL:
//       return s << termcolor::bright_red << "F";
//   }
// }

// void MyPrefixFormatter(std::ostream& s, const google::LogMessage& m, void* /*data*/) {
//   using namespace std;
//   s << m.severity()
//     << setw(4) << 1900 + m.time().year()
//     << setw(2) << 1 + m.time().month()
//     << setw(2) << m.time().day()
//     << ' '
//     << setw(2) << m.time().hour() << ':'
//     << setw(2) << m.time().min()  << ':'
//     << setw(2) << m.time().sec() << "."
//     << setw(6) << m.time().usec()
//     << ' '
//     << setfill(' ') << setw(5)
//     << m.thread_id() << setfill('0')
//     << ' '
//     << m.basename() << ':' << m.line() << "]";
// }

auto main(int argc, char** argv) -> int {
  Runtime::Init(argc, argv);

  uword counter = 0;
  keyboard::OnKeyPressedEvent()
    .subscribe(LogEvent<keyboard::KeyPressedEvent>());
  keyboard::OnKeyPressedEvent()
    .filter(keyboard::KeyPressedEvent::FilterBy(GLFW_KEY_GRAVE_ACCENT))
    .subscribe([](keyboard::KeyPressedEvent* e) {
      PrintRuntimeInfo();
    });
  return Runtime::Run();
}