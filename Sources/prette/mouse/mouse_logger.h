#ifdef PRT_DEBUG

#ifndef PRT_MOUSE_H
#error "Please #include <prette/mouse/mouse.h> instead."
#endif //PRT_MOUSE_H

#ifndef PRT_MOUSE_LOGGER_H
#define PRT_MOUSE_LOGGER_H

#include "prette/mouse/mouse.h"
#include "prette/pretty_logger.h"

namespace prt::mouse {
  class MouseLogger : public PrettyLogger {
  public:
    MouseLogger(const google::LogSeverity severity,
                const char* file,
                const int line,
                const int indent):
      PrettyLogger(severity, file, line, indent) {
    }
    explicit MouseLogger(PrettyLogger* parent):
      PrettyLogger(parent) {  
    }
    ~MouseLogger() override = default;
    void Print();
  };
}

#endif //PRT_MOUSE_LOGGER_H
#endif //PRT_DEBUG