#include "prette/shader/shader.h"

#include <cstddef>
#include <filesystem>
#include <fmt/format.h>
#include <optional>
#include <string>
#include <units.h>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/shader/shader_builder.h"
#include "prette/shader/shader_code.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
static inline auto GetFilenameWithoutExtension(const fs::path& path) -> std::string {
  std::string filename = path.filename();
  const auto dotpos = filename.find_first_of('.');
  if (dotpos != std::string::npos)
    filename = filename.substr(0, filename.length() - dotpos);
  return filename;
}

Shader::Shader(std::string name, const VkShaderModuleCreateInfo& create_info) :
  prt::vk::NamedHandleTemplate<HandleType>(name) {
  const auto driver = Driver::Get();
  driver->CreateShaderModule(&create_info, handle_ptr());
}

Shader::~Shader() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  driver->DestroyShaderModule(handle_ref());
}

auto Shader::ToString() const -> std::string {
  ToStringHelper<Shader> helper{};
  helper.AddField("name", GetName());
  return helper;
}

static inline auto IsValidShaderFile(const fs::path path) -> bool {
  return fs::exists(path) && fs::is_regular_file(path);
}

auto ResolveShaderCodePath(std::string p) -> std::optional<fs::path> {
  if (!p.ends_with(".spv"))
    p = p + ".spv";
  EnvironmentVariable env_path("PRT_PATH");
  std::vector<std::string> paths{};
  env_path >> paths;
  for (const auto& path : paths) {
    fs::path file = fmt::format("{}/shaders/{}", path, p);
    if (!IsValidShaderFile(file))
      continue;
    return {file};
  }
  return std::nullopt;
}

auto FindShaderCode(std::string name) -> ShaderCode {
  if (!name.ends_with(".spv"))
    name = name + ".spv";
  EnvironmentVariable path("PRT_PATH");
  std::vector<std::string> paths{};
  path >> paths;
  for (const auto& p : paths) {
    fs::path file = fmt::format("{}/shaders/{}", p, name);
    if (!IsValidShaderFile(file))
      continue;
    return ShaderCode(file);
  }
  DLOG(WARNING) << "failed to find code for shader named `" << name
                << "` in the following paths: " << path.Get().value_or("");
  return {};
}

auto GetVertexShader(const std::string name) -> Shader* {
  ASSERT(!name.empty());
  const auto code = FindVertexShaderCode(name);
  LOG_IF(FATAL, !code) << "failed to find pipeline vertex shader code";
  ShaderBuilder builder{};
  // clang-format off
  return builder.WithName(std::move(name))
    .WithShaderCode(code)
    .Build();
  // clang-format on
}

auto GetFragmentShader(const std::string name) -> Shader* {
  ASSERT(!name.empty());
  const auto code = FindFragmentShaderCode(name);
  LOG_IF(FATAL, !code) << "failed to find pipeline vertex shader code";
  ShaderBuilder builder{};
  // clang-format off
  return builder
    .WithName(std::move(name))
    .WithShaderCode(code)
    .Build();
  // clang-format on
}
}  // namespace prt::vk