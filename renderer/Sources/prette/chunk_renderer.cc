#include "prette/chunk_renderer.h"

#include <array>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/camera_manager.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/chunk_mesh.h"
#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/descriptor_set_layout.h"
#include "prette/descriptor_set_update.h"
#include "prette/material/material.h"
#include "prette/material/material_system.h"
#include "prette/pipeline/pipeline.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/tile.h"
#include "prette/tile_mesh.h"
#include "prette/uniform_buffer.h"
#include "prette/vk.h"

namespace prt {
static inline auto CreateTileDescriptors() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder;
  builder.WithName("tiles");
  builder.AddBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);  // tiles
  return builder.Build();
}

static inline auto NewMaterialBuffer() -> vk::Buffer* {
  vk::UniformBufferBuilder<TileData> builder(kTotalNumberOfTilesPerChunk);
  builder.WithTransferDestUsage().WithStorageUsage();
  return builder;
}

ChunkRenderer::ChunkRenderer() {
  OnInitDescriptorSets([this](InitDescriptorSetsEvent* event) {
    {
      vk::DescriptorSetLayoutBuilder builder{};
      builder.AddStorageBufferBinding().WithStageFlags(VK_SHADER_STAGE_VERTEX_BIT);
      tile_descriptors_layout_ = builder;
    }
  });
  OnInitGraphicsPipelines([this](InitGraphicsPipelinesEvent* event) {
    tile_pipeline_ = vk::RenderPipeline::FromJson("tile.json");
    ASSERT_INITIALIZED(tile_pipeline_);
  });
  OnInitBuffers([this](InitBuffersEvent* event) {
    tile_mesh_ = NewTileMesh();
    ASSERT_INITIALIZED(tile_mesh_);
    const auto mat_sys = MaterialSystem::GetSystem();
    const auto num_mats = mat_sys->GetNumberOfMaterialsLoaded();
    material_buffers_.resize(num_mats);
    tile_descriptors_.resize(num_mats);
    for (auto idx = 0; idx < num_mats; idx++) {
      material_buffers_[idx] = NewMaterialBuffer();
      tile_descriptors_[idx] = new vk::DescriptorSet("tile", *tile_descriptors_layout_);

      {
        vk::DescriptorSetUpdate update(tile_descriptors_[idx]);
        update.AddWriteStorageBuffer(0).WithBufferInfo(material_buffers_[idx]);
      }
    }
  });
}

ChunkRenderer::~ChunkRenderer() {
  delete tile_mesh_;
  delete texture_;
}

void ChunkRenderer::RenderChunkMesh(VkCommandBuffer buffer, ChunkMesh* mesh, const uint64_t num_instances) {
  ASSERT_INITIALIZED(mesh);
  ASSERT(num_instances >= 1);
  mesh->Bind(buffer);
  vkCmdDrawIndexed(buffer, ChunkMeshClass::kTotalNumberOfIndices, num_instances, 0, 0, 0);
}

class TileMeshifier : public TileVisitor {
  DEFINE_NON_COPYABLE_TYPE(TileMeshifier);

 private:
  std::vector<std::vector<TileData>>& results_;

 public:
  explicit TileMeshifier(std::vector<std::vector<TileData>>& results, const uint64_t num_materials) :
    TileVisitor(),
    results_(results) {
    results_.resize(num_materials);
    for (auto idx = 0; idx < num_materials; idx++)
      results_[idx].reserve(kTotalNumberOfTilesPerChunk);
  }
  ~TileMeshifier() override = default;

  auto Visit(Tile* tile) -> bool override {
    ASSERT(tile);
    results_[tile->GetMaterial()].push_back(tile->data());
    return true;
  }
};

void ChunkRenderer::Render(VkCommandBuffer buffer, Chunk* chunk) {
  const auto mat_system = MaterialSystem::GetSystem();
  const auto num_materials = mat_system->GetNumberOfMaterialsLoaded();

  std::vector<std::vector<TileData>> data{};
  TileMeshifier meshifier(data, num_materials);
  LOG_IF(FATAL, !chunk->VisitTiles(&meshifier)) << "failed to meshify chunk.";

  tile_pipeline_->Bind(&buffer);
  tile_mesh_->Bind(buffer);
  for (auto idx = 0; idx < num_materials; idx++) {
    const auto& tiles = data[idx];
    const auto& mat_buffer = material_buffers_[idx];
    const auto material = mat_system->GetMaterial(idx);
    vk::CopyBytesToBufferWithStaging::Copy(&tiles[0], tiles.size(), mat_buffer);
    std::array<VkDescriptorSet, 3> draw_descriptors = {
        *GetCameraManager()->GetDescriptorSet(),
        *material->GetDescriptorSet(),
        *tile_descriptors_[idx],
    };
    vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *tile_pipeline_->GetPipelineLayout(), 0,
                            draw_descriptors.size(), &draw_descriptors[0], 0, nullptr);
    vkCmdDrawIndexed(buffer, ChunkMeshClass::kTotalNumberOfIndices, tiles.size(), 0, 0, 0);
  }
}
}  // namespace prt