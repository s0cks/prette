#ifndef PRT_SHADER_CODE_H
#define PRT_SHADER_CODE_H

#include <cstdint>
#include <fmt/format.h>
#include <string>
#include <vector>

#include "prette/common.h"

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

static inline auto FindVertexShaderCode(std::string filename) -> ShaderCode {
  ASSERT(!filename.empty());
  return FindShaderCode(fmt::format("{}.vert", filename));
}

static inline auto FindFragmentShaderCode(std::string filename) -> ShaderCode {
  ASSERT(!filename.empty());
  return FindShaderCode(fmt::format("{}.frag", filename));
}
}  // namespace prt::vk

#endif  // PRT_SHADER_CODE_H
