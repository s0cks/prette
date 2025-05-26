#ifndef PRT_SHADER_H
#define PRT_SHADER_H

#include <filesystem>
#include <fmt/format.h>
#include <optional>
#include <string>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {
auto GetVertexShader(const std::string name) -> Shader*;
auto GetFragmentShader(const std::string name) -> Shader*;
auto ResolveShaderCodePath(std::string p) -> std::optional<fs::path>;

class Shader : public vk::NamedHandleTemplate<VkShaderModule> {
  using HandleType = VkShaderModule;
  friend class ShaderBuilder;
  DEFINE_NON_COPYABLE_TYPE(Shader);

 private:
  Shader(std::string name, const VkShaderModuleCreateInfo& info);

 public:
  ~Shader() override;

  auto ToString() const -> std::string override;
  operator HandleType() const {
    return GetHandle();
  }
};
}  // namespace prt::vk

#endif  // PRT_SHADER_H
