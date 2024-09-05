#include "prette/program/program_uniform.h"
#include "prette/program/program.h"

namespace prt::program {
  UniformIterator::UniformIterator(const Program* program):
    program_(program),
    cur_uniform_(0),
    num_uniforms_(0),
    length_(0),
    size_(0),
    type_(0),
    name_asize_(0),
    name_(nullptr) {
    PRT_ASSERT(program);
    const auto num_uniforms = program->GetNumberOfActiveUniforms();
    if(num_uniforms <= 0) {
      DLOG(WARNING) << "no uniforms for: " << program->ToString();
      return;
    }
    
    const auto name_asize = program->GetActiveUniformsMaxLength();
    PRT_ASSERT(name_asize >= 1);
    const auto name = (char*) malloc(sizeof(char) * name_asize);
    PRT_ASSERT(name);

    num_uniforms_ = num_uniforms;
    name_ = name;
    name_asize_ = name_asize;
  }

  UniformIterator::~UniformIterator() {
    if(name_)
      free(name_);
  }

  ProgramId UniformIterator::GetProgramId() const {
    return GetProgram()->GetId();
  }

  bool UniformIterator::HasNext() const {
    return cur_uniform_ < num_uniforms_;
  }

  ProgramUniform UniformIterator::Next() {
    memset(name_, 0, sizeof(name_));
    glGetActiveUniform(GetProgramId(), cur_uniform_, name_asize_, &length_, &size_, &type_, (GLchar*) name_);
    CHECK_GL;
    const auto uniform = ProgramUniform(cur_uniform_, type_, size_, name_, length_);
    cur_uniform_++;
    return uniform;
  }
}