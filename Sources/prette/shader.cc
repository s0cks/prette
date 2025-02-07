#include "prette/shader.h"

#include <vulkan/vulkan_core.h>

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

#ifdef PRT_VK
void CreateShaderModule(const VkDevice& device, const std::vector<char>& code, VkShaderModule& shader_module) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size();
  create_info.pCode = ((const uint32_t*)code.data());
  CHECK_VK(FATAL, vkCreateShaderModule(device, &create_info, nullptr, &shader_module), "failed to create vk shader module");
}
#endif  // PRT_VK
}  // namespace prt