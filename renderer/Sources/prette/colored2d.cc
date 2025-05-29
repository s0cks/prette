#include "prette/colored2d.h"

namespace prt {
// void Colored2DPipeline::PushRect(const glm::vec2 top_left, const glm::vec2 bottom_left, const glm::vec2 bottom_right,
//                                  const glm::vec2 top_right) {
//   const auto top_left_pos = AppendVertex({
//       .pos = top_left,
//       .color = color_,
//   });
//   const auto bottom_left_pos = AppendVertex({
//       .pos = bottom_left,
//       .color = color_,
//   });
//   const auto bottom_right_pos = AppendVertex({
//       .pos = bottom_right,
//       .color = color_,
//   });
//   const auto top_right_pos = AppendVertex({
//       .pos = top_right,
//       .color = color_,
//   });
//   AppendIndex(top_left_pos);
//   AppendIndex(bottom_left_pos);
//   AppendIndex(bottom_right_pos);
//   AppendIndex(top_left_pos);
//   AppendIndex(bottom_right_pos);
//   AppendIndex(top_right_pos);
//   num_instances_ += 1;
// }

// void Colored2DPipeline::Draw(VkCommandBuffer buffer) {
//   ASSERT(buffer != VK_NULL_HANDLE);
//   vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *GetPipelineLayout(), 0, 1,
//                           &GetDescriptors()->GetHandle(), 0, nullptr);
//   Bind(&buffer);
//   std::array<VkDeviceSize, 1> offsets = {0};
//   vkCmdBindVertexBuffers(buffer, 0, 1, &GetVertexBuffer()->GetBuffer(), offsets.data());
//   vkCmdBindIndexBuffer(buffer, GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
//   vkCmdDrawIndexed(buffer, GetNumberOfIndices(), 1, 0, 0, 0);
// }
}  // namespace prt