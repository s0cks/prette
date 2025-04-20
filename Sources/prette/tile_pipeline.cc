#include "prette/tile_pipeline.h"

#include "prette/tile_vertex.h"

namespace prt {

auto TileGraphicsPipelineBuilder::Build() -> TileGraphicsPipeline* {
  std::array<VkVertexInputBindingDescription, 1> bindings = {
      TileVertex::GetBindingDescription(),
  };
  InitInput(vertex_input_, input_assembly_, bindings, TileVertex::GetAttributeDescriptions());

  VkPipeline pipeline{};
  VkGraphicsPipelineCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info.stageCount = shader_attachments_.size();
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
  return new TileGraphicsPipeline(render_pass_, extent_, pipeline, layout_, cache_);
}
}  // namespace prt