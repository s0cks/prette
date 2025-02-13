#include "prette/shader.h"

#include <fmt/format.h>
#include <vulkan/vulkan_core.h>

#include <filesystem>

#include "prette/common.h"

namespace prt {
auto ReadShaderCode(const std::string& filename, std::vector<char>& code) -> bool {
  ASSERT(!filename.empty());
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open())
    return false;
  auto filesize = static_cast<size_t>(file.tellg());
  code.resize(filesize);
  file.seekg(0);
  file.read(code.data(), filesize);
  file.close();
  return true;
}

auto CreateShaderModule(const VkDevice& device, const std::vector<char>& code, VkShaderModule& shader_module) -> bool {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = ((const uint32_t*)code.data());
  CHECK_VK(FATAL, vkCreateShaderModule(device, &create_info, nullptr, &shader_module), "failed to create vk shader module");
  return true;
}

auto CreateShaderModule(const VkDevice& device, const std::string& filename, VkShaderModule& shader_module) -> bool {
  EnvironmentVariable path("PRT_PATH");

  std::vector<std::string> paths;
  path.GetList(paths);

  std::vector<char> code;
  for (const auto& path : paths) {
    const auto file = fmt::format("{}/shaders/{}", path, filename);
    if (!fs::exists(file) || !fs::is_regular_file(file))
      continue;
    if (!ReadShaderCode(file, code)) {
      LOG(ERROR) << "failed to read shader code from: " << file;
      continue;
    }
  }

  if (code.empty()) {
    LOG(ERROR) << "failed to find shader code for: " << filename;
    return false;
  }

  return CreateShaderModule(device, code, shader_module);
}
}  // namespace prt