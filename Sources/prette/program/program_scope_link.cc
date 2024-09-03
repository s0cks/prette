#include "prette/program/program_scope.h"
#include "prette/program/program.h"

namespace prt::program {
  ProgramLinkScope::ProgramLinkScope(Program* program):
    ProgramScope(program),
    linked_() {
  }

  ProgramLinkScope::~ProgramLinkScope() {
    for(const auto& linked : linked_)
      Program::Detach(GetProgramId(), linked);
  }

  void ProgramLinkScope::Attach(const ShaderId id) {
    Program::Attach(GetProgramId(), id);
    linked_.push_back(id);
  }

  void ProgramLinkScope::AttachAll(const ShaderId* ids, const uword num_ids) {
    for(auto idx = 0; idx < num_ids; idx++)
      Attach(ids[idx]);
  }

  ProgramLinkStatus ProgramLinkScope::Link() const {
    const auto id = GetProgramId();
    glLinkProgram(id);
    CHECK_GL(FATAL);
    return ProgramLinkStatus(id);
  }
}