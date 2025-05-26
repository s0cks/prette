#ifndef PRT_SHADER_BUILDER_H
#define PRT_SHADER_BUILDER_H

#include <cstdint>

#include "prette/shader/shader_code.h"
#include "prette/vk.h"

namespace prt::vk {
class ShaderBuilder : public vk::NamedHandleBuilderTemplate<VkShaderModuleCreateInfo, Shader, ShaderBuilder> {
  using ParentType = NamedHandleBuilderTemplate<VkShaderModuleCreateInfo, Shader, ShaderBuilder>;

 public:
  ShaderBuilder();
  ~ShaderBuilder() override = default;

  auto WithShaderCode(const uint32_t* bytes, const uint64_t num_bytes) -> ShaderBuilder& {
    info_ptr()->codeSize = num_bytes;
    info_ptr()->pCode = bytes;
    return *this;
  }

  inline auto WithShaderCode(const ShaderCode& rhs) -> ShaderBuilder& {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    return WithShaderCode((const uint32_t*)rhs.data(), rhs.GetSize());
  }

  auto WithNext(const void* rhs) -> ShaderBuilder& {
    info_ptr()->pNext = rhs;
    return *this;
  }

  auto WithFlags(const VkShaderModuleCreateFlags rhs) -> ShaderBuilder& {
    info_ptr()->flags = rhs;
    return *this;
  }

  auto IsValid() const -> bool override;
  auto Build() -> Shader* override;
};
}  // namespace prt::vk

#endif  // PRT_SHADER_BUILDER_H
