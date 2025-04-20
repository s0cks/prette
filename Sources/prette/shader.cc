#include "prette/shader.h"

#include <fmt/format.h>
#include <vulkan/vulkan_core.h>

#include <filesystem>
#include <ios>

#include "prette/common.h"

namespace prt {
ShaderCode::ShaderCode(fs::path path) :
  path_(path) {
  if (!fs::exists(path)) {
    LOG(ERROR) << "failed to find ShaderCode at: " << path;
    return;
  }
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    LOG(ERROR) << "failed to open ShaderCode at: " << path;
    return;
  }
  auto filesize = static_cast<std::streamsize>(file.tellg());
  DVLOG(1) << "loading shader code from " << path << " (" << units::data::byte_t(static_cast<double>(filesize)) << ")...";
  buffer_.resize(filesize);
  file.seekg(0);
  file.read((char*)buffer_.data(), filesize);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  file.close();
}

static inline auto GetFilenameWithoutExtension(const fs::path& path) -> std::string {
  std::string filename = path.filename();
  const auto dotpos = filename.find_first_of('.');
  if (dotpos != std::string::npos)
    filename = filename.substr(0, filename.length() - dotpos);
  return filename;
}

Shader::Shader(std::string name, const ShaderCode& code) :
  name_(std::move(name)) {
  const auto driver = Driver::Get();
  ASSERT(driver);
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.GetSize();
  create_info.pCode = ((const uint32_t*)code.data());
  const auto result = vkCreateShaderModule(driver->GetDevice(), &create_info, driver->GetAllocator(), &handle_);
  LOG_IF(FATAL, result != VK_SUCCESS) << "failed to create " << name << " shader: " << result;
}

Shader::~Shader() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  vkDestroyShaderModule(driver->GetDevice(), GetHandle(), driver->GetAllocator());
}

static inline auto IsValidShaderFile(const fs::path path) -> bool {
  return fs::exists(path) && fs::is_regular_file(path);
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
    return {file};
  }
  DLOG(WARNING) << "failed to find code for shader named `" << name << "` in the following paths: " << path.Get().value_or("");
  return {};
}

auto GetVertexShader(const std::string& name) -> ShaderPtr {
  ASSERT(!name.empty());
  const auto code = FindVertexShaderCode(name);
  LOG_IF(FATAL, !code) << "failed to find pipeline vertex shader code";
  return std::make_shared<Shader>(fmt::format("{}-vtx", name), code);
}

auto GetFragmentShader(const std::string& name) -> ShaderPtr {
  ASSERT(!name.empty());
  const auto code = FindFragmentShaderCode(name);
  LOG_IF(FATAL, !code) << "failed to find pipeline vertex shader code";
  return std::make_shared<Shader>(fmt::format("{}-frag", name), code);
}
}  // namespace prt