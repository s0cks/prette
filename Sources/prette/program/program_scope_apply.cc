#include "prette/program/program_scope.h"
#include "prette/program/program.h"

namespace prt::program {
  ApplyProgramScope::ApplyProgramScope(Program* program):
    ProgramScope(program) {  
    Apply();
  }

  ApplyProgramScope::~ApplyProgramScope() {
    Unapply();
  }

  UniformLocation ApplyProgramScope::GetUniformLocation(const char* name) const {
    const auto location = glGetUniformLocation(GetProgramId(), name);
    CHECK_GL;
    return location;
  }

  UniformBlockIndex ApplyProgramScope::GetUniformBlockIndex(const char* name) const {
    const auto index = glGetUniformBlockIndex(GetProgramId(), name);
    CHECK_GL;
    return index;
  }

  void ApplyProgramScope::Set(const char* name, const glm::vec2& value) const {
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    CHECK_GL;
  }

  void ApplyProgramScope::Set(const char* name, const glm::vec3& value) const {
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    CHECK_GL;
  }

  void ApplyProgramScope::Set(const char* name, const glm::vec4& value) const {
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
    CHECK_GL;
  }

  void ApplyProgramScope::Set(const char* name, const glm::mat4& value, const bool transpose) const {
    glUniformMatrix4fv(GetUniformLocation(name), 1, transpose, glm::value_ptr(value));
    CHECK_GL;
  }

  void ApplyProgramScope::Set(const char* name, const float value) const {
    glUniform1f(GetUniformLocation(name), value);
    CHECK_GL;
  }

  void ApplyProgramScope::Set(const char* name, const int32_t value) const {
    glUniform1i(GetUniformLocation(name), value);
    CHECK_GL;
  }

  void ApplyProgramScope::Set(const char* name, const uint32_t value) const {
    glUniform1ui(GetUniformLocation(name), value);
    CHECK_GL;
  }

  void ApplyProgramScope::Apply() {
    Program::UseProgram(GetProgramId());
  }

  void ApplyProgramScope::Unapply() {
    Program::UseProgram(kInvalidProgramId);
  }
}