#ifndef PRT_SHADER_H
#define PRT_SHADER_H

#include <fmt/format.h>

#include <fstream>
#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
class ShaderCode {
  using ShaderCodeBuffer = std::vector<uint8_t>;
  DEFINE_DEFAULT_COPYABLE_TYPE(ShaderCode);

 private:
  fs::path path_;
  ShaderCodeBuffer buffer_{};

 public:
  ShaderCode() = default;
  ShaderCode(fs::path path);
  ~ShaderCode() = default;

  auto GetPath() const -> const fs::path& {
    return path_;
  }

  auto GetSize() const -> uint64_t {
    return buffer_.size();
  }

  auto data() const -> const uint8_t* {
    return buffer_.data();
  }

  auto IsEmpty() const -> bool {
    return buffer_.empty();
  }

  auto IsValid() const -> bool {
    return !IsEmpty();
  }

  operator bool() const {
    return IsValid();
  }
};

auto FindShaderCode(std::string name) -> ShaderCode;

static inline auto FindVertexShaderCode(std::string filename) -> ShaderCode {
  ASSERT(!filename.empty());
  return FindShaderCode(fmt::format("{}.vert", filename));
}

static inline auto FindFragmentShaderCode(std::string filename) -> ShaderCode {
  ASSERT(!filename.empty());
  return FindShaderCode(fmt::format("{}.frag", filename));
}

class Shader {
  using Handle = VkShaderModule;
  DEFINE_NON_COPYABLE_TYPE(Shader);

 private:
  std::string name_;
  Handle handle_{};

 public:
  Shader(std::string name, const ShaderCode& code);
  ~Shader();

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto GetHandle() const -> const Handle& {
    return handle_;
  }

  operator Handle() const {
    return handle_;
  }
};

using ShaderPtr = std::shared_ptr<Shader>;

auto GetVertexShader(const std::string& name) -> ShaderPtr;
auto GetFragmentShader(const std::string& name) -> ShaderPtr;

static inline void InitPipelineShaderStage(VkPipelineShaderStageCreateInfo& create_info, ShaderPtr shader,
                                           const VkShaderStageFlagBits stage, const char* name = "main") {
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  create_info.stage = stage;
  create_info.module = shader->GetHandle();
  create_info.pName = name;
}

static inline void InitPipelineVertexShaderStage(VkPipelineShaderStageCreateInfo& create_info, ShaderPtr shader,
                                                 const char* name = "main") {
  return InitPipelineShaderStage(create_info, shader, VK_SHADER_STAGE_VERTEX_BIT, name);
}

static inline void InitPipelineFragmentShaderStage(VkPipelineShaderStageCreateInfo& create_info, ShaderPtr shader,
                                                   const char* name = "main") {
  return InitPipelineShaderStage(create_info, shader, VK_SHADER_STAGE_FRAGMENT_BIT, name);
}
}  // namespace prt

#endif  // PRT_SHADER_H
