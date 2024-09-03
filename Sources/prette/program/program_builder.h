#ifndef PRT_PROGRAM_BUILDER_H
#define PRT_PROGRAM_BUILDER_H

#include "prette/rx.h"
#include "prette/builder.h"
#include "prette/shader/shader.h"
#include "prette/program/program_id.h"

namespace prt::program {
  using shader::ShaderSet;

  class Program;
  class ProgramBuilder : public BuilderTemplate<Program> {
  protected:
    ShaderSet shaders_;
  public:
    ProgramBuilder() = default;
    ~ProgramBuilder() override = default;

    const ShaderSet& GetShaders() const {
      return shaders_;
    }

    virtual bool Attach(Shader* shader);

    Program* Build() const override;
    rx::observable<Program*> BuildAsync() const override;
  };
}

#endif //PRT_PROGRAM_BUILDER_H