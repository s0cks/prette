#include "prette/shader/shader_source_attacher.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  ShaderSourceAttacher::ShaderSourceAttacher(ShaderUnit* unit):
    ShaderCodeVisitor(),
    sources_(),
    lengths_(),
    num_attached_(0) {
    if(unit)
      Append(unit);
  }

  void ShaderSourceAttacher::Append(ShaderCode* code) {
    PRT_ASSERT(code);
    sources_.push_back((GLchar*) code->GetData()); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    lengths_.push_back((GLint) code->GetLength());
    num_attached_ += 1;
  }

  void ShaderSourceAttacher::Append(ShaderUnit* unit) {
    PRT_ASSERT(unit);
    const auto num_sources = unit->GetSize();
    sources_.reserve(num_attached_ + num_sources);
    lengths_.reserve(num_attached_ + num_sources);
    LOG_IF(ERROR, !unit->Accept(this)) << "failed to attach sources from: " << unit;
  }

  auto ShaderSourceAttacher::AttachSources(const ShaderId target) -> bool {
    PRT_ASSERT(target);
    VLOG(1) << "attaching " << num_attached_ << " sources to Shader #" << target;
    for(auto idx = 0; idx < num_attached_; idx++)
      VLOG(1) << " - source #" << (idx + 1) << "/" << num_attached_ << ": " << std::endl << GetSourceAt(idx);
    glShaderSource(target, (GLsizei) num_attached_, &sources_[0], &lengths_[0]);
    CHECK_GL;
    return true;
  } 
}