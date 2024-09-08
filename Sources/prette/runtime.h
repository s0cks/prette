#ifndef PRT_RUNTIME_H
#define PRT_RUNTIME_H

#include <glog/logging.h>
#include "prette/common.h"

namespace prt {
#ifdef PRT_DEBUG

  void PrintRuntimeInfo(const google::LogSeverity severity = google::INFO,
                        const char* file = __FILE__,
                        const int line = __LINE__,
                        const int indent = 0);

#endif //PRT_DEBUG

  class Runtime {
    friend class RuntimeInfoPrinter;
    DEFINE_NON_INSTANTIABLE_TYPE(Runtime);
  private:
    static void OnUnhandledException();
  public:
    static void Init(int argc, char** argv);
    static auto Run() -> int;
  };
}

#endif //PRT_RUNTIME_H