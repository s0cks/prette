#include "prette/mouse/mouse.h"
#ifdef PRT_DEBUG

namespace prt::mouse {
#define __ google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << std::string(indent_ * 2, ' ')
  void MouseLogger::Print() {
    //TODO: better debug information
    __ << "Mouse:";
    Indent();
    if(IsMouseDisabled()) {
      __ << "State: Disabled";
    } else {
      __ << "State: Enabled.";
    }
    Deindent();
  }
#undef __
}

#endif //PRT_DEBUG