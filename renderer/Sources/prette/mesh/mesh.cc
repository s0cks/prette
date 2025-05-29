#include "prette/mesh/mesh.h"

#include <vulkan/vulkan_core.h>

namespace prt::vk {

// void Mesh::Render(VkCommandBuffer buffer) {
// std::array<VkDeviceSize, 1> offsets = {0};
// vkCmdBindVertexBuffers(buffer, 0, 1, &GetVertexBuffer()->GetBuffer(), offsets.data());
// vkCmdDrawIndexed(buffer, kNumberOfIndicesPerInstance, 1, 0, 0, 0);
// }

// void IndexedMesh::Render(VkCommandBuffer buffer) {
// std::array<VkDeviceSize, 1> offsets = {0};
// vkCmdBindVertexBuffers(buffer, 0, 1, &GetVertexBuffer()->GetBuffer(), offsets.data());
// vkCmdBindIndexBuffer(buffer, GetIndexBuffer()->GetBuffer(), 0, M::IndexClass::kFormat);
// vkCmdDrawIndexed(buffer, kNumberOfIndicesPerInstance, 1, 0, 0, 0);
// }
}  // namespace prt::vk