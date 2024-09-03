#include "prette/program/program_pipeline.h"

namespace prt::program {
  const ApplyProgramPipeline::SetUniformsCallback ApplyProgramPipeline::kDoNothing = [](ApplyProgramScope& ref) {
    // do nothing.
  };

  bool ApplyProgramPipeline::Apply() {
    ApplyProgramScope scope(GetProgram());
    set_uniforms_(scope);
    return true;
  }
}