#include "prette/mesh/mesh.h"

#include <cstdint>
#include <vulkan/vulkan_core.h>

#include "prette/index_buffer.h"
#include "prette/vertex/vertex_buffer.h"

namespace prt::vk {
void Mesh::Bind(VkCommandBuffer cmd) {
  vk::BindVertexBuffer(cmd, GetVertexBuffer());
}

void IndexedMesh::Bind(VkCommandBuffer cmd) {
  Mesh::Bind(cmd);
  vk::BindIndexBuffer<uint16_t>(cmd, GetIndexBuffer());
}
}  // namespace prt::vk