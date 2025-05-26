#include "prette/shader/shader_builder.h"

#include "prette/shader/shader.h"

namespace prt::vk {
ShaderBuilder::ShaderBuilder() :
  ParentType() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info_ptr()->codeSize = 0;
  info_ptr()->pCode = nullptr;
  info_ptr()->flags = 0;
  info_ptr()->pNext = nullptr;
}

auto ShaderBuilder::IsValid() const -> bool {
  return ParentType::IsValid();
}

auto ShaderBuilder::Build() -> Shader* {
  ASSERT(IsValid());
  return new Shader(GetName(), info());
}
}  // namespace prt::vk