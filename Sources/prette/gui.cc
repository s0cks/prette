#include "prette/gui.h"

#include <imgui.h>
#include <vulkan/vulkan_core.h>

namespace prt::gui {
struct VkFont {
  VkImage image{};
  VkDeviceMemory memory{};
  VkImageView view{};
};

static VkPipeline pipeline_{};
static VkPipelineLayout pipeline_layout_{};
static VkDescriptorPool descriptor_pool_{};
static VkDescriptorSet descriptor_set_{};
static VkSampler sampler_{};
static VkFont font_{};
static VkDescriptorSetLayout descriptor_set_layout_{};

static inline void InitStyle() {
  ImGuiStyle& style = ImGui::GetStyle();
}

static inline void InitResources() {
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* fontData = nullptr;
}

struct PushConstBlock {
  glm::vec2 scale;
  glm::vec2 translate;
};

static inline void InitPipeline(VkPipelineCache pipelineCache, VkRenderPass renderPass) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PushConstBlock);

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &descriptor_set_layout_;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
  pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
}

void Init(const Dimension& size) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(static_cast<float>(size.width()), static_cast<float>(size.height()));
  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

  InitStyle();
  InitResources();
  // TODO:  InitPipeline();
}
}  // namespace prt::gui