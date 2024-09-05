#include "prette/program/program_attribute.h"
#include "prette/program/program.h"

namespace prt::program {
  AttributeIterator::AttributeIterator(const Program* program):
    program_(program),
    cur_attr_(0),
    num_attrs_(0),
    length_(0),
    size_(0),
    type_(0),
    name_asize_(0),
    name_(nullptr) {
    PRT_ASSERT(program);
    const auto num_attrs = program->GetNumberOfActiveAttributes();
    if(num_attrs <= 0) {
      DLOG(WARNING) << "no attributes for: " << program->ToString();
      return;
    }
    
    const auto name_asize = program->GetActiveAttributesMaxLength();
    PRT_ASSERT(name_asize >= 1);
    const auto name = (char*) malloc(sizeof(char) * name_asize);
    PRT_ASSERT(name);

    num_attrs_ = num_attrs;
    name_ = name;
    name_asize_ = name_asize;
  }

  AttributeIterator::~AttributeIterator() {
    if(name_)
      free(name_);
  }

  ProgramId AttributeIterator::GetProgramId() const {
    return GetProgram()->GetId();
  }

  bool AttributeIterator::HasNext() const {
    return cur_attr_ < num_attrs_;
  }

  ProgramAttribute AttributeIterator::Next() {
    memset(name_, 0, sizeof(name_));
    glGetActiveAttrib(GetProgramId(), cur_attr_, name_asize_, &length_, &size_, &type_, (GLchar*) name_);
    CHECK_GL;
    const auto attr = ProgramAttribute(cur_attr_, type_, size_, name_, length_);
    cur_attr_++;
    return attr;
  }
}