#ifndef PRT_PROGRAM_PIPELINE_H
#define PRT_PROGRAM_PIPELINE_H

#include "prette/pipeline.h"
#include "prette/program/program.h"
#include "prette/program/program_scope.h"

namespace prt::program {
  class ProgramPipeline : public Pipeline {
  protected:
    Program* program_;

    explicit ProgramPipeline(Program* program):
      Pipeline(),
      program_(program) {
    }
  public:
    ~ProgramPipeline() override = default;

    Program* GetProgram() const {
      return program_;
    }
  };

  class ApplyProgramPipeline : public ProgramPipeline {
  public:
    typedef std::function<void(ApplyProgramScope&)> SetUniformsCallback;
    static const SetUniformsCallback kDoNothing;
  private:
    SetUniformsCallback set_uniforms_;

    ApplyProgramPipeline(Program* program, SetUniformsCallback callback):
      ProgramPipeline(program),
      set_uniforms_(callback) {
    }
  public:
    ~ApplyProgramPipeline() override = default;

    const char* GetName() const override {
      return "ApplyProgram";
    }

    bool Apply() override;
  public:
    static inline ApplyProgramPipeline*
    New(Program* program, SetUniformsCallback callback = kDoNothing) {
      return new ApplyProgramPipeline(program, callback);
    }
  };
}

#endif //PRT_PROGRAM_PIPELINE_H