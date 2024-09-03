#include "prette/program/program_printer.h"
#include "prette/program/program_status.h"

namespace prt::program {
#define __ google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << GetIndentString()

  bool ProgramPrinter::VisitProgram(Program* program) {
    PRT_ASSERT(program);
    const auto id = program->GetId();
    __ << "Program:";
    Indent();
    {
      __ << "Id: " << program->GetId();
      const auto link_status = ProgramLinkStatus(id);
      __ << "Linked: " << (link_status ? 'y' : 'n');
      const auto delete_status = ProgramDeleteStatus(id);
      __ << "Deleted: " << (delete_status ? 'y' : 'n');
      const auto validate_status = ProgramValidateStatus(id);
      __ << "Validated: " << (validate_status ? 'y' : 'n');
      const auto num_attrs = program->GetNumberOfActiveAttributes();
      if(num_attrs > 0) {
        __ << "Attributes (" << num_attrs << "):";
        Indent();
        {
          program->VisitAllAttributes(this);
        }
        Deindent();
      }
      const auto num_uniforms = program->GetNumberOfActiveUniforms();
      if(num_uniforms > 0) {
        __ << "Uniforms (" << num_uniforms << "):";
        Indent();
        {
          program->VisitAllUniforms(this);
        }
        Deindent();
      }
    }
    Deindent();
    return true;
  }

  bool ProgramPrinter::VisitAttribute(const ProgramAttribute& attr) {
    __ << "- " << attr;
    return true;
  }

  bool ProgramPrinter::VisitUniform(const ProgramUniform& uniform) {
    __ << "- " << uniform;
    return true;
  }
#undef __
}