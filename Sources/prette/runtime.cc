#include "prette/runtime.h"

#include "prette/prette.h"
#include "prette/mouse/mouse.h"
#include "prette/engine/engine.h"
#include "prette/pretty_logger.h"

namespace prt {
#ifdef PRT_DEBUG

#define __  google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << std::string(indent_ * 2, ' ')
  class RuntimeInfoPrinter : public PrettyLogger,
                             public shader::ShaderVisitor,
                             public vao::VaoVisitor,
                             public vbo::VboVisitor,
                             public ibo::IboVisitor,
                             public fbo::FboVisitor,
                             public texture::TextureVisitor,
                             public program::ProgramVisitor {
    using Severity=google::LogSeverity;
  private:
    inline bool PrintObject(Object* obj) {
      PRT_ASSERT(obj);
      __ << "- " << obj->ToString();
      return true;
    }

    inline void PrintEngineInfo() {
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
    inline void PrintAllShaders() {
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

    inline void PrintAllPrograms() {
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

    inline void PrintAllVaos() {
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

    inline void PrintAllVbos() {
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

    inline void PrintAllIbos() {
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

    inline void PrintAllFbos() {
      __ << "Frame Buffer Objects:";
      Indent();
      {
        __ << "Total: " << fbo::GetTotalNumberOfFbos();
        fbo::VisitAllFbos(this);
      }
      Deindent();
    }

    inline void PrintAllTextures() {
      __ << "Textures:";
      Indent();
      {
        __ << "Total: " << texture::GetTotalNumberOfTextures();
        texture::VisitAllTextures(this);
      }
      Deindent();
    }

    inline void PrintMouseInfo() {
      mouse::MouseLogger logger(this);
      return logger.Print();
    }
  public:
    explicit RuntimeInfoPrinter(const Severity severity = google::INFO,
                                const char* file = __FILE__,
                                const int line = __LINE__,
                                const int indent = 0):
      PrettyLogger(severity, file, line, indent),
      shader::ShaderVisitor(),
      vao::VaoVisitor(),
      vbo::VboVisitor(),
      ibo::IboVisitor() {
    }
    ~RuntimeInfoPrinter() override = default;

    Severity GetSeverity() const {
      return severity_;
    }

#define DEFINE_VISIT_SHADER(Name, Ext, GlValue)                       \
    bool Visit##Name##Shader(Name##Shader* shader) override {         \
      return PrintObject(shader);                                     \
    }
    FOR_EACH_SHADER_TYPE(DEFINE_VISIT_SHADER)
#undef DEFINE_VISIT_SHADER

    bool VisitVao(Vao* vao) override {
      return PrintObject(vao);
    }

    bool VisitVbo(Vbo* vbo) override {
      return PrintObject(vbo);
    }

    bool VisitIbo(Ibo* ibo) override {
      return PrintObject(ibo);
    }

    bool VisitFbo(Fbo* fbo) override {
      return PrintObject(fbo);
    }

    bool VisitProgram(Program* program) override {
      return PrintObject(program);
    }

    bool VisitTexture(Texture* texture) override {
      return PrintObject(texture);
    }

    void Print() {
      google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << "Runtime Information:";
      __ << "Version: " << prt::GetVersion() << " (" << PRT_GIT_BRANCH << "/" << PRT_GIT_COMMIT_HASH << ")";
      __ << "Resources: " << FLAGS_resources;
      __ << "GLFW Version: ";
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
  };
#undef __

  void PrintRuntimeInfo(const google::LogSeverity s, const char* file, const int line, const int indent) {
    RuntimeInfoPrinter printer(s, file, line, indent);
    return printer.Print();
  }
#endif //PRT_DEBUG
}