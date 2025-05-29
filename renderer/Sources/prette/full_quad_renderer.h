#ifndef PRT_FULL_QUAD_RENDERER_H
#define PRT_FULL_QUAD_RENDERER_H

// #include <cstdint>
// #include <type_traits>
// #include <vulkan/vulkan_core.h>

// #include "prette/descriptor_set.h"
// #include "prette/mesh.h"
// #include "prette/mesh_class.h"
// #include "prette/pipeline/pipeline.h"
// #include "prette/std140.h"
// #include "prette/vertex/vertex2d.h"
// #include "prette/vk.h"

// namespace prt {
// struct FullQuadData {
//   glm::mat4 model{};
// };
// template <>
// struct vk::is_mesh_data_t<FullQuadData> : std::true_type {};

// template <>
// struct std140::is_aligned<FullQuadData> : std::true_type {};

// struct FullQuadMeshClass : public vk::MeshClass<FullQuadData, color2d::Vertex, 4, uint16_t, 6> {
//   static constexpr const auto kClassName = "full-quad-color2d";
// };

// using FullQuadMesh = vk::SingleMesh<FullQuadMeshClass>;

// class FullQuadRenderer {
//  private:
//   vk::RenderPipeline* pipeline_ = nullptr;
//   FullQuadMesh* mesh_ = nullptr;

//  public:
//   FullQuadRenderer();
//   ~FullQuadRenderer();

//   auto GetMesh() const -> FullQuadMesh* {
//     return mesh_;
//   }

//   auto GetPipeline() const -> vk::RenderPipeline* {
//     return pipeline_;
//   }

//   auto IsInitialized() const -> bool {
//     return vk::AllInitialized(pipeline_) && mesh_ != nullptr;
//   }

//   void Render(VkCommandBuffer buffer);
// };

// void InitFullQuadRenderer();
// auto IsFullQuadRendererInitialized() -> bool;
// }  // namespace prt

#endif  // PRT_FULL_QUAD_RENDERER_H
