#include "prette/pipeline_builder.h"

#include "prette/pipeline.h"
#include "prette/renderer.h"

namespace prt {
void PipelineCacheBuilder::Build(VkPipelineCache& cache) {
  VkPipelineCacheCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  Driver::Get()->CreatePipelineCache(create_info, cache);
}

void PipelineLayoutBuilder::Build(VkPipelineLayout& layout) {
  VkPipelineLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  create_info.setLayoutCount = descriptor_set_layouts_.size();
  create_info.pSetLayouts = descriptor_set_layouts_.data();
  create_info.pushConstantRangeCount = 0;
  create_info.pPushConstantRanges = nullptr;
  Driver::Get()->CreatePipelineLayout(create_info, layout);
}

auto GraphicsPipelineBuilder::Build() -> GraphicsPipeline* {
  std::array<VkVertexInputBindingDescription, 1> bindings = {
      Vertex::GetBindingDescription(),
  };
  InitInput(vertex_input_, input_assembly_, bindings, Vertex::GetAttributeDescriptions());

  VkPipeline pipeline{};
  VkGraphicsPipelineCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info.stageCount = 2;
  create_info.pStages = &shader_attachments_[0];
  create_info.pVertexInputState = &vertex_input_;
  create_info.pInputAssemblyState = &input_assembly_;
  create_info.pViewportState = &viewport_;
  create_info.pRasterizationState = &rasterizer_;
  create_info.pMultisampleState = &multisampling_;
  create_info.pColorBlendState = &blending_;
  create_info.pDynamicState = &dynamic_state_;
  create_info.pDepthStencilState = &depth_stencil_;
  create_info.layout = layout_;
  create_info.renderPass = render_pass_;
  create_info.subpass = 0;
  create_info.basePipelineHandle = VK_NULL_HANDLE;
  create_info.basePipelineIndex = -1;
  Driver::Get()->CreateGraphicsPipeline(create_info, cache_, pipeline);
  return new GraphicsPipeline(render_pass_, extent_, pipeline, layout_, cache_);
}
}  // namespace prt