#ifndef PRT_PROGRAM_PIPELINE_H
#define PRT_PROGRAM_PIPELINE_H

#include <utility>

#include "prette/pipeline.h"
#include "prette/program/program.h"
#include "prette/program/program_scope.h"

namespace prt::program {
  class ProgramPipeline : public Pipeline {
    DEFINE_NON_COPYABLE_TYPE(ProgramPipeline);
  private:
    Program* program_;
  protected:
    explicit ProgramPipeline(Program* program):
      Pipeline(),
      program_(program) {
    }
  public:
    ~ProgramPipeline() override = default;

    auto GetProgram() const -> Program* {
      return program_;
    }
  };

  class ApplyProgramPipeline : public ProgramPipeline {
    DEFINE_NON_COPYABLE_TYPE(ApplyProgramPipeline);
  public:
    using SetUniformsCallback = std::function<void(ApplyProgramScope&)>;
    static const SetUniformsCallback kDoNothing;
  private:
    SetUniformsCallback set_uniforms_;

    ApplyProgramPipeline(Program* program, SetUniformsCallback callback):
      ProgramPipeline(program),
      set_uniforms_(std::move(callback)) {
    }
  public:
    ~ApplyProgramPipeline() override = default;

    auto GetName() const -> const char* override {
      return "ApplyProgram";
    }

    auto Apply() -> bool override;
  public:
    static inline auto
    New(Program* program, const SetUniformsCallback& callback = kDoNothing) -> ApplyProgramPipeline* {
      return new ApplyProgramPipeline(program, callback);
    }
  };
}

#endif //PRT_PROGRAM_PIPELINE_H