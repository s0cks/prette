#include "prette/signals.h"
#include <glog/logging.h>

namespace prt {
  static inline void
  OnInterrupt(const int signal) {
    LOG(FATAL) << "interrupt.";
  }

  static inline void
  OnAbort(const int signal) {
    LOG(FATAL) << "abort.";
  }

  static inline void
  OnSegfault(const int signal) {
    LOG(FATAL) << "segfault.";
  }

  static inline void
  OnTerminate(const int signal) {
    LOG(FATAL) << "terminate.";
  }

  typedef void* (*SignalHandler)(const int);

  template<const Signal S>
  static inline void
  OnSignal(sig_t handler) {
    const auto bound = std::signal(S, handler);
    LOG_IF(FATAL, bound == SIG_ERR) << "failed to bind SignalHandler for the " << S << " signal.";
    DLOG(INFO) << "registered SignalHandler for " << S;
  }

  void InitSignalHandlers() {
    DLOG(INFO) << "initializing signal handlers....";
    OnSignal<kInterruptSignal>(&OnInterrupt);
    OnSignal<kAbortSignal>(&OnAbort);
    OnSignal<kSegfaultSignal>(&OnSegfault);
    OnSignal<kTerminateSignal>(&OnTerminate);
  }
}