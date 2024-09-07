#include "prette/program/program_scope.h"
#include "prette/program/program.h"

namespace prt::program {
  auto ProgramScope::GetProgramId() const -> ProgramId {
    return GetProgram()->GetId();
  }

  void ProgramUboBindScope::Bind(const std::string& name, Ubo* ubo) {
    PRT_ASSERT(ubo);
    const auto index = index_++;
    const auto bound = BoundUbo {
      .index = index,
      .name = name,
      .ubo = ubo,
    };
    const auto inserted = bound_.insert(bound);
    if(!inserted.second) {
      LOG(ERROR) << "failed to insert: " << bound;
      return;
    }
    glBindBufferRange(GL_UNIFORM_BUFFER, index, ubo->GetId(), 0, ubo->GetTotalSize());
    CHECK_GL;
    GetProgram()->SetUniformBlock(name, index);
  }
}