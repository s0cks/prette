#include "prette/program/program_attribute.h"
#include "prette/program/program.h"

namespace prt::program {
  AttributeIterator::AttributeIterator(const Program* program):
    program_(program),
    name_() {
    PRT_ASSERT(program);
    const auto num_attrs = program->GetNumberOfActiveAttributes();
    if(num_attrs <= 0) {
      DLOG(WARNING) << "no attributes for: " << program->ToString();
      return;
    }
    num_attrs_ = num_attrs;
    const auto name_asize = program->GetActiveAttributesMaxLength();
    PRT_ASSERT(name_asize >= 1);
    name_.reserve(name_asize);
  }
  
  auto AttributeIterator::GetProgramId() const -> ProgramId {
    return GetProgram()->GetId();
  }

  auto AttributeIterator::HasNext() const -> bool {
    return index_ < num_attrs_;
  }

  auto AttributeIterator::Next() -> ProgramAttribute {
    name_.clear();
    glGetActiveAttrib(GetProgramId(), index_, (GLsizei) name_.capacity(), &length_, &size_, &type_, &name_[0]);
    CHECK_GL;
    return { index_++, type_, size_, (const char*) &name_[0], length_ };
  }
}