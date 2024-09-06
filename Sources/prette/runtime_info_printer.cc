#include "prette/runtime_info_printer.h"
#ifdef PRT_DEBUG

#include "prette/prette.h"
#include "prette/runtime.h"
#include "prette/mouse/mouse.h"
#include "prette/engine/engine.h"

#include "prette/query/query.h"
#include "prette/query/query_scope.h"

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

  //TODO: Get individual stats for shader types
  void RuntimeInfoPrinter::PrintAllShaders() {
    __ << "Shaders:";
    Indent();
    {
      __ << "Total: " << shader::GetTotalNumberOfShaders();
      {
        __ << "Fragment Shaders: ";
        Indent();
        shader::VisitAllFragmentShaders(this);
        Deindent();
      }
      {
        __ << "Vertex Shaders: ";
        Indent();
        shader::VisitAllVertexShaders(this);
        Deindent();
      }
      {
        __ << "Geometry Shaders: ";
        Indent();
        shader::VisitAllGeometryShaders(this);
        Deindent();
      }
      {
        __ << "TessEval Shaders: ";
        Indent();
        shader::VisitAllTessEvalShaders(this);
        Deindent();
      }
      {
        __ << "TessControl Shaders: ";
        Indent();
        shader::VisitAllTessControlShaders(this);
        Deindent();
      }
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintAllPrograms() {
    __ << "Programs";
    Indent();
    {
      __ << "Total: " << program::GetTotalNumberOfPrograms();
      Indent();
      program::VisitAllPrograms(this);
      Deindent();
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintAllVaos() {
    __ << "Vertex Array Objects:";
    Indent();
    {
      __ << "Total: " << vao::GetTotalNumberOfVaos();
      Indent();
      vao::VisitAllVaos(this);
      Deindent();
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintAllVbos() {
    __ << "Vertex Buffer Objects:";
    Indent();
    {
      __ << "Total: " << vbo::GetTotalNumberOfVbos();
      Indent();
      vbo::VisitAllVbos(this);
      Deindent();
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintAllIbos() {
    __ << "Index Buffer Objects:";
    Indent();
    {
      __ << "Total: " << ibo::GetTotalNumberOfIbos();
      Indent();
      ibo::VisitAllIbos(this);
      Deindent();
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintAllFbos() {
    __ << "Frame Buffer Objects:";
    Indent();
    {
      __ << "Total: " << fbo::GetTotalNumberOfFbos();
      fbo::VisitAllFbos(this);
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintAllTextures() {
    __ << "Textures:";
    Indent();
    {
      __ << "Total: " << texture::GetTotalNumberOfTextures();
      Indent();
      texture::VisitAllTextures(this);
      Deindent();
    }
    Deindent();
  }

  void RuntimeInfoPrinter::PrintMouseInfo() {
    mouse::MouseLogger logger(this);
    return logger.Print();
  }
  
  void RuntimeInfoPrinter::Print() {
    google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << "Runtime Information:";
    __ << "Version: " << prt::GetVersion() << " (" << PRT_GIT_BRANCH << "/" << PRT_GIT_COMMIT_HASH << ")";
    __ << "Resources: " << FLAGS_resources;
    __ << "OpenGL:";
    Indent();
    {
      __ << "Version: " << gfx::GetGlVersionString();
      __ << "Vendor: " << gfx::GetGlVendorString();
      __ << "Renderer: " << gfx::GetGlRendererString();
      __ << "GLFW Version: " << gfx::GetGlfwVersion();
      __ << "GLSL Version: " << gfx::GetGlslVersionString();
      //TODO: print GPU time
    }
    Deindent();
#ifndef OS_IS_OSX
    __ << "GLEW Version: " << gfx::GetGlewVersion();
#endif //OS_IS_OSX
    PrintEngineInfo();
    PrintMouseInfo();
    PrintAllShaders();
    PrintAllPrograms();
    PrintAllVaos();
    PrintAllVbos();
    PrintAllIbos();
    PrintAllFbos();
    PrintAllTextures();
  }
#undef __
}

#endif //PRT_DEBUG