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
    DEFINE_NON_COPYABLE_TYPE(ProgramBuilder);
  private:
    ShaderSet shaders_;
  public:
    ProgramBuilder() = default;
    ~ProgramBuilder() override = default;

    auto GetShaders() const -> const ShaderSet& {
      return shaders_;
    }

    virtual auto Attach(Shader* shader) -> bool;

    auto Build() const -> Program* override;
    auto BuildAsync() const -> rx::observable<Program*> override;
  };
}

#endif //PRT_PROGRAM_BUILDER_H