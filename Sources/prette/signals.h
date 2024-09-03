#ifndef PRT_SIGNALS_H
#define PRT_SIGNALS_H

#include <ostream>
#include <csignal>

namespace prt {
#define FOR_EACH_SIGNAL(V)          \
  V(Abort,              SIGABRT)    \
  V(FpError,            SIGFPE)     \
  V(IllegalInstr,       SIGILL)     \
  V(Interrupt,          SIGINT)     \
  V(Segfault,           SIGSEGV)    \
  V(Terminate,          SIGTERM)

  enum Signal : int {
    kNone = 0,
#define DEFINE_SIGNAL(Name, Value)  k##Name##Signal = (Value),
    FOR_EACH_SIGNAL(DEFINE_SIGNAL)
#undef DEFINE_SIGNAL
  };

  static inline constexpr const char*
  GetSignalName(const Signal& rhs) {
    switch(rhs) {
#define DEFINE_GET_NAME(Name, Value)           \
      case k##Name##Signal: return #Name;
      FOR_EACH_SIGNAL(DEFINE_GET_NAME)
#undef DEFINE_GET_NAME
      default: return "Unknown Signal.";
    }
  }

  static inline std::ostream&
  operator<<(std::ostream& stream, const Signal& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, Value)                                                           \
      case k##Name##Signal: return stream << #Name << " (" << #Value << ":" << (Value) << ")";
      FOR_EACH_SIGNAL(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown Signal #" << (static_cast<int>(rhs)) << ".";
    }
  }

  void InitSignalHandlers();
}

#endif //PRT_SIGNALS_H