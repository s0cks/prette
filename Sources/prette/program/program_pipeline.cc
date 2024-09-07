#include "prette/program/program_pipeline.h"

namespace prt::program {
  const ApplyProgramPipeline::SetUniformsCallback ApplyProgramPipeline::kDoNothing = [](ApplyProgramScope& ref) {
    // do nothing.
  };

  auto ApplyProgramPipeline::Apply() -> bool {
    ApplyProgramScope scope(GetProgram());
    set_uniforms_(scope);
    return true;
  }
}