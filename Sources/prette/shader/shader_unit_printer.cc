#include "prette/shader/shader_unit_printer.h"

namespace prt::shader {
#define __ \
  google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << GetIndentString()
  auto ShaderUnitPrinter::VisitShaderCode(ShaderCode* code) -> bool {
    PRT_ASSERT(code);
    __ << "- " << code->ToString();
    return true;
  }

  void ShaderUnitPrinter::PrintShaderUnit(ShaderUnit* unit) {
    PRT_ASSERT(unit);
    const auto& meta = unit->GetMeta();
    __ << "Name: " << meta.GetName();
    __ << "Type: " << unit->GetType();
    __ << "Hash: " << unit->GetHash();
    __ << "Sources (" << unit->GetSize() << "):";
    Indent();
    unit->Accept(this);
    Deindent();
  }
#undef __
}