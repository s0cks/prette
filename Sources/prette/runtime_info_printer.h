#ifndef PRT_RUNTIME_INFO_PRINTER_H
#define PRT_RUNTIME_INFO_PRINTER_H
#ifdef PRT_DEBUG

#include "prette/pretty_logger.h"
#include "prette/vao/vao.h"
#include "prette/vbo/vbo.h"
#include "prette/ibo/ibo.h"
#include "prette/fbo/fbo.h"
#include "prette/shader/shader.h"
#include "prette/program/program.h"
#include "prette/texture/texture.h"

namespace prt {
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
    bool PrintObject(Object* obj);
    void PrintEngineInfo();
    void PrintAllShaders();
    void PrintAllPrograms();
    void PrintAllVaos();
    void PrintAllVbos();
    void PrintAllIbos();
    void PrintAllFbos();
    void PrintAllTextures();
    void PrintMouseInfo();
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

    void Print();
  };
}

#endif //PRT_DEBUG
#endif //PRT_RUNTIME_INFO_PRINTER_H