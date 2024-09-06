#include "prette/program/program_builder.h"

#include <glog/logging.h>

#include "prette/common.h"
#include "prette/shader/shader.h"
#include "prette/program/program.h"
#include "prette/program/program_scope.h"
#include "prette/program/program_status.h"

#include "prette/program/program_printer.h"

namespace prt::program {
  bool ProgramBuilder::Attach(Shader* shader) {
    PRT_ASSERT(shader);
    const auto [iter,success] = shaders_.insert(shader);
    if(!success) {
      LOG(ERROR) << "failed to attach: " << shader->ToString();
      return false;
    }
    DLOG(INFO) << "attached: " << shader->ToString();
    return true;
  }

  Program* ProgramBuilder::Build() const { //TODO: code cleanup
    // create program
    const auto id = glCreateProgram();
    CHECK_GL;
    if(!IsValidProgramId(id)) {
      DLOG(ERROR) << "failed to create program.";
      return nullptr;
    }

    const auto program = new Program(GetMeta(), id);
    ProgramLinkScope link_scope(program);
    for(const auto& shader : GetShaders())
      link_scope << shader->GetId();
    const auto status = link_scope.Link();
    if(!status) {
      DLOG(ERROR) << "failed to link Program #" << id << ": " << status;
      return nullptr; //TODO: cleanup
    }
    DLOG(INFO) << status;
    PRINT_PROGRAM(INFO, program);
    return program;
  }

  rx::observable<Program*> ProgramBuilder::BuildAsync() const {
    return rx::observable<>::create<Program*>([this](rx::subscriber<Program*> s) {
      const auto program = Build();
      if(!program) {
        const auto err = "failed to build Program.";
        return s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
      }
      s.on_next(program);
      s.on_completed();
    });
  }
}