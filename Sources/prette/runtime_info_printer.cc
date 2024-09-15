#include "prette/runtime_info_printer.h"
#ifdef PRT_DEBUG

#include "prette/prette.h"
#include "prette/runtime.h"
#include "prette/mouse/mouse.h"
#include "prette/engine/engine.h"

#include "prette/window/monitor.h"

namespace prt {
#define __  google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << std::string(indent_ * 2, ' ')
  bool RuntimeInfoPrinter::PrintObject(Object* obj) {
    PRT_ASSERT(obj);
    __ << "- " << obj->ToString();
    return true;
  }

  void RuntimeInfoPrinter::PrintEngineInfo() {
    const auto engine = GetEngine();
    if(!engine) {
      __ << "Engine: n/a";
      return;
    }
    __ << "Engine: ";
    Indent();
    {
      const auto state = engine->GetState();
      __ << "State: " << (state ? state->GetName() : "n/a");
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintMouseInfo() {
    mouse::MouseLogger logger(this);
    return logger.Print();
  }

  void RuntimeInfoPrinter::Print() {
    google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << "Runtime Information:";
    __ << "Version: " << prt::GetVersion() << " (" << PRT_GIT_BRANCH << "/" << PRT_GIT_COMMIT << ")";
    __ << "Resources: " << FLAGS_resources;
    __ << "OpenGL:";
    Indent();
    {
      __ << "GLFW Version: " << gfx::GetGlfwVersion();
    }
    Deindent();
#ifndef OS_IS_OSX
    __ << "GLEW Version: " << gfx::GetGlewVersion();
#endif //OS_IS_OSX
    PrintEngineInfo();
    PrintMouseInfo();
    {
      __ << "Monitors:";
      Indent();
      window::PrintAllMonitors(GetSeverity(), GetFile(), GetLine(), GetIndent());
      Deindent();
    }
  }
#undef __
}

#endif //PRT_DEBUG