#include "prette/shader/shader_unit_printer.h"

namespace prt::shader {
#define __ \
  google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << GetIndentString()
  bool ShaderUnitPrinter::VisitShaderCode(ShaderCode* code) {
    PRT_ASSERT(code);
    __ << "- " << code->ToString();
    return true;
  }

  void ShaderUnitPrinter::PrintShaderUnit(ShaderUnit* unit) {
    PRT_ASSERT(unit);
    __ << unit->GetType() << " Shader Unit: ";
    const auto& meta = unit->GetMeta();
    __ << "Name: " << meta.GetName();
    __ << "Hash: " << unit->GetHash();
    __ << "Sources (" << unit->GetSize() << "):";
    Indent();
    {
      unit->Accept(this);
    }
    Deindent();
  }
#undef __
}