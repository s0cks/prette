#ifndef PRT_SHADER_H
#define PRT_SHADER_H

#include <filesystem>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/vk.h"

namespace prt::vk {
class ShaderCode {
  using ShaderCodeBuffer = std::vector<uint8_t>;
  DEFINE_DEFAULT_COPYABLE_TYPE(ShaderCode);

 private:
  fs::path path_;
  ShaderCodeBuffer buffer_{};

 public:
  ShaderCode() = default;
  explicit ShaderCode(fs::path path);
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
auto GetVertexShader(const std::string name) -> Shader*;
auto GetFragmentShader(const std::string name) -> Shader*;
auto ResolveShaderCodePath(std::string p) -> std::optional<fs::path>;

static inline auto FindVertexShaderCode(std::string filename) -> ShaderCode {
  ASSERT(!filename.empty());
  return FindShaderCode(fmt::format("{}.vert", filename));
}

static inline auto FindFragmentShaderCode(std::string filename) -> ShaderCode {
  ASSERT(!filename.empty());
  return FindShaderCode(fmt::format("{}.frag", filename));
}

class Shader;
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
