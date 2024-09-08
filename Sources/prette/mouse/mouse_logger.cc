#include "prette/mouse/mouse.h"
#ifdef PRT_DEBUG

namespace prt::mouse {
#define __ google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << std::string(indent_ * 2, ' ')
  void MouseLogger::Print() {
    __ << "Mouse:";
    Indent();
    __ << "Mode: " << GetMouseMode();
    Deindent();
  }
#undef __
}

#endif //PRT_DEBUG