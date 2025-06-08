#ifndef PRT_PIPELINE_BUILDER_H
#define PRT_PIPELINE_BUILDER_H

#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/pipeline/pipeline_cache.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/platform.h"
#include "prette/vk.h"

namespace prt {
namespace json {
class PipelineHandler;
}
namespace vk {
static constexpr const VkExtent2D kInvalidRenderPipelineExtent{0, 0};

class PipelineShaderStageBuilder {
  friend class BaseRenderPipelineBuilder;
  DEFINE_DEFAULT_COPYABLE_TYPE(PipelineShaderStageBuilder);

 private:
  std::string name_{};
  VkPipelineShaderStageCreateInfo* data_ = nullptr;

  explicit PipelineShaderStageBuilder(VkPipelineShaderStageCreateInfo* d, const VkShaderStageFlagBits stage) :
    data_(d) {
    ASSERT(data_);
    data()->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    data()->module = VK_NULL_HANDLE;
    data()->pName = nullptr;
    data()->stage = stage;
  }

  inline auto data() const -> VkPipelineShaderStageCreateInfo* {
    return data_;
  }

 public:
  PipelineShaderStageBuilder() = default;
  ~PipelineShaderStageBuilder() = default;

  auto WithName(const char* name) -> PipelineShaderStageBuilder& {
    data()->pName = name;
    return *this;
  }

  auto WithModule(VkShaderModule rhs) -> PipelineShaderStageBuilder& {
    data()->module = rhs;
    return *this;
  }

  auto WithFlags(const VkPipelineShaderStageCreateFlags rhs) -> PipelineShaderStageBuilder& {
    data()->flags = rhs;
    return *this;
  }

  auto WithNext(const void* rhs) -> PipelineShaderStageBuilder& {
    data()->pNext = rhs;
    return *this;
  }
};

class BaseRenderPipelineBuilder {
  friend class prt::json::PipelineHandler;

 protected:
  static inline void InitViewportAndScissor(VkViewport& viewport, VkRect2D& scissor, const VkExtent2D& extent,
                                            const glm::fvec2& pos = glm::fvec2(0.0f), const float min_depth = 0.0f,
                                            const float max_depth = 1.0f) {
    // viewport
    viewport.x = pos.x;
    viewport.y = pos.y;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = min_depth;
    viewport.maxDepth = max_depth;
    // scissor
    scissor.offset = {.x = static_cast<int32_t>(pos.x), .y = static_cast<int32_t>(pos.y)};
    scissor.extent = extent;
  }

  static inline void InitViewportCreateInfo(VkPipelineViewportStateCreateInfo& create_info, const VkViewport& viewport,
                                            const VkRect2D& scissor) {
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    create_info.viewportCount = 1;
    create_info.pViewports = &viewport;
    create_info.scissorCount = 1;
    create_info.pScissors = &scissor;
  }

  static inline void InitRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer) {
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
  }

  static inline void InitMultisampling(VkPipelineMultisampleStateCreateInfo& multisampling) {
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
  }

  static inline void InitDynamicState(VkPipelineDynamicStateCreateInfo& create_info,
                                      const std::vector<VkDynamicState>& dynamic_states) {
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    create_info.dynamicStateCount = dynamic_states.size();
    create_info.pDynamicStates = dynamic_states.data();
  }

  template <typename AttachmentsContainer>
  static inline void InitBlending(VkPipelineColorBlendStateCreateInfo& create_info,
                                  const AttachmentsContainer& attachments) {
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    create_info.logicOpEnable = VK_FALSE;
    create_info.logicOp = VK_LOGIC_OP_COPY;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.blendConstants[0] = 0.0f;
    create_info.blendConstants[1] = 0.0f;
    create_info.blendConstants[2] = 0.0f;
    create_info.blendConstants[3] = 0.0f;
  }

  template <typename BindingsContainer, typename AttributesContainer>
  static inline void InitInput(VkPipelineVertexInputStateCreateInfo& vertex_input,
                               VkPipelineInputAssemblyStateCreateInfo& input_assembly,
                               const BindingsContainer& bindings, const AttributesContainer& attributes) {
    vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input.vertexBindingDescriptionCount = bindings.size();
    vertex_input.pVertexBindingDescriptions = bindings.data();
    vertex_input.vertexAttributeDescriptionCount = attributes.size();
    vertex_input.pVertexAttributeDescriptions = attributes.data();

    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
  }

  static inline void InitColorBlendAttachment(VkPipelineColorBlendAttachmentState& color_blend_attachment) {
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
  }

  static inline void InitDepthStencil(VkPipelineDepthStencilStateCreateInfo& depth_stencil) {
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.minDepthBounds = 0.0f;
    depth_stencil.maxDepthBounds = 1.0f;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = {};
    depth_stencil.back = {};
  }

 protected:
  std::string name_{};
  VkViewport vp_{};
  VkRect2D scissor_{};
  VkPipelineViewportStateCreateInfo viewport_{};
  VkPipelineDepthStencilStateCreateInfo depth_stencil_{};
  VkPipelineInputAssemblyStateCreateInfo input_assembly_{};
  VkPipelineVertexInputStateCreateInfo vertex_input_{};
  VkPipelineRasterizationStateCreateInfo rasterizer_{};
  VkPipelineMultisampleStateCreateInfo multisampling_{};
  VkPipelineColorBlendStateCreateInfo blending_{};
  VkPipelineDynamicStateCreateInfo dynamic_state_{};
  VkGraphicsPipelineCreateInfo pipeline_{};
  std::vector<Shader*> attached_shaders_{};
  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_{};
  std::vector<VkPipelineColorBlendAttachmentState> blending_attachments_{};
  std::vector<VkDynamicState> dynamic_states_{};
  std::vector<VkVertexInputBindingDescription> vertex_bindings_{};
  std::vector<VkVertexInputAttributeDescription> vertex_attrs_{};

  vk::PipelineLayout* layout_ = nullptr;
  vk::PipelineCache* cache_ = nullptr;
  VkRenderPass render_pass_ = VK_NULL_HANDLE;
  VkExtent2D extent_;

  BaseRenderPipelineBuilder(const VkExtent2D extent) :
    extent_(extent) {
    InitDepthStencil(depth_stencil_);
    InitViewportAndScissor(vp_, scissor_, extent_);
    InitViewportCreateInfo(viewport_, vp_, scissor_);

    InitRasterizer(rasterizer_);
    InitMultisampling(multisampling_);

    blending_attachments_.resize(6);
    for (auto idx = 0; idx < 6; idx++) {
      InitColorBlendAttachment(blending_attachments_[idx]);
    }
    InitBlending(blending_, blending_attachments_);

    attached_shaders_.reserve(2);

    InitDynamicState(dynamic_state_, dynamic_states_);
    InitPipelineInfo(pipeline_);
  }

  void InitPipelineInfo(VkGraphicsPipelineCreateInfo& info);
  void InitPipeline(VkPipeline& pipeline);

 public:
  virtual ~BaseRenderPipelineBuilder() = default;

  void SetName(const std::string rhs) {
    ASSERT(!rhs.empty());
    name_ = std::move(rhs);
  }

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  void SetExtent(const VkExtent2D& extent) {
    extent_ = extent;
    InitViewportAndScissor(vp_, scissor_, extent_);
    InitViewportCreateInfo(viewport_, vp_, scissor_);
  }

  auto GetRenderPass() const -> const VkRenderPass& {
    return render_pass_;
  }

  void SetRenderPass(VkRenderPass rhs) {
    ASSERT(rhs != VK_NULL_HANDLE);
    render_pass_ = rhs;
  }

  void SetViewport(const VkViewport rhs) {
    vp_ = rhs;
  }

  inline auto HasRenderPass() const -> bool {
    return GetRenderPass() != VK_NULL_HANDLE;
  }

  auto GetPipelineLayout() const -> vk::PipelineLayout* {
    return layout_;
  }

  void SetPipelineLayout(vk::PipelineLayout* layout);

  inline auto HasPipelineLayout() const -> bool {
    return GetPipelineLayout() != VK_NULL_HANDLE;
  }

  auto GetPipelineCache() const -> vk::PipelineCache* {
    return cache_;
  }

  void SetPipelineCache(vk::PipelineCache* rhs) {
    ASSERT(rhs);
    cache_ = rhs;
  }

  inline auto HasPipelineCache() const -> bool {
    return GetPipelineCache() != VK_NULL_HANDLE;
  }

  void SetRasterizerCullMode(const VkCullModeFlags rhs);
  void SetRasterizerPolygonMode(const VkPolygonMode rhs);
  void SetRasterizerCullMode(const std::string& rhs);

  void SetLineWidth(const float rhs);

  void SetRasterizerFrontFace(const VkFrontFace rhs);
  void SetRasterizerFrontFace(const std::string& rhs);

  void AttachVertexShader(Shader* shader);
  void AttachFragmentShader(Shader* shader);

  void AddVertexBindings(const VkVertexInputBindingDescription* data, const uint64_t num_bindings);
  void AddVertexBinding(const VkVertexInputBindingDescription rhs) {
    return AddVertexBindings(&rhs, 1);
  }

  template <typename BindingsContainer>
  inline void AddVertexBindings(const BindingsContainer& rhs) {
    return AddVertexBindings(rhs.data(), rhs.size());
  }

  void AddVertexAttributes(const VkVertexInputAttributeDescription* data, const uint64_t num_attrs);

  template <typename AttrsContainer>
  inline void AddVertexAttributes(const AttrsContainer& rhs) {
    return AddVertexAttributes(rhs.data(), rhs.size());
  }

  auto WithShaderStage(const VkShaderStageFlagBits stage) -> PipelineShaderStageBuilder {
    const auto idx = shader_stages_.size();
    shader_stages_.resize(idx + 1);
    return PipelineShaderStageBuilder(&shader_stages_[idx], stage);
  }

  inline auto WithVertexShaderStage() -> PipelineShaderStageBuilder {
    return WithShaderStage(VK_SHADER_STAGE_VERTEX_BIT);
  }

  inline auto WithFragmentShaderStage() -> PipelineShaderStageBuilder {
    return WithShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT);
  }
};

template <typename T>
class TemplateRenderPipelineBuilder : public BaseRenderPipelineBuilder {
 protected:
  explicit TemplateRenderPipelineBuilder(const VkExtent2D extent) :
    BaseRenderPipelineBuilder(std::move(extent)) {}

 public:
  ~TemplateRenderPipelineBuilder() override = default;
  virtual auto Build() -> T* = 0;
};
}  // namespace vk
}  // namespace prt

#endif  // PRT_PIPELINE_BUILDER_H
