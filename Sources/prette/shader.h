#ifndef PRT_SHADER_H
#define PRT_SHADER_H

#include <vulkan/vulkan_core.h>

#include <fstream>
#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
auto ReadShaderCode(const std::string& filename, std::vector<char>& code) -> bool;

#ifdef PRT_VK
void CreateShaderModule(const VkDevice& device, const std::vector<char>& code, VkShaderModule& shader_module);

static inline void InitShaderStageCreateInfo(VkPipelineShaderStageCreateInfo& create_info, VkShaderModule m,
                                             const VkShaderStageFlagBits stage, const char* name = "main") {
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  create_info.stage = stage;
  create_info.module = m;
  create_info.pName = name;
}

static inline void InitVertexShaderStageCreateInfo(VkPipelineShaderStageCreateInfo& create_info, const VkShaderModule& m,
                                                   const char* name = "main") {
  return InitShaderStageCreateInfo(create_info, m, VK_SHADER_STAGE_VERTEX_BIT, name);
}

static inline void InitFragmentShaderStageCreateInfo(VkPipelineShaderStageCreateInfo& create_info, const VkShaderModule& m,
                                                     const char* name = "main") {
  return InitShaderStageCreateInfo(create_info, m, VK_SHADER_STAGE_FRAGMENT_BIT, name);
}
#endif  // PRT_VK
}  // namespace prt

#endif  // PRT_SHADER_H
