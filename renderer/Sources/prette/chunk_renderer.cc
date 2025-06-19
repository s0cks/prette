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
  builder.AddStorageBufferBinding(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);  // tiles
  return builder.Build();
}

static inline auto NewMaterialBuffer() -> vk::Buffer* {
  vk::UniformBufferBuilder<TileData> builder(kTotalNumberOfTilesPerChunk * 16);
  builder.WithTransferDestUsage().WithStorageUsage();
  return builder;
}

ChunkRenderer::ChunkRenderer() {
  OnInitDescriptorSets([this](InitDescriptorSetsEvent* event) {
    {
      vk::DescriptorSetLayoutBuilder builder{};
      builder.AddStorageBufferBinding().WithStageFlags(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
      tile_descriptors_layout_ = builder;
    }
    {
      vk::DescriptorSetBuilder builder{};
      builder.WithName("chunk");
      builder.AddStorageBufferBinding(VK_SHADER_STAGE_VERTEX_BIT);
      chunk_descriptors_ = builder;
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
      ASSERT_INITIALIZED(material_buffers_[idx]);
      tile_descriptors_[idx] = new vk::DescriptorSet("tile", *tile_descriptors_layout_);
      ASSERT_INITIALIZED(tile_descriptors_[idx]);
      {
        vk::DescriptorSetUpdate update(tile_descriptors_[idx]);
        update.AddWriteStorageBuffer(0).WithBufferInfo(material_buffers_[idx]);
      }
    }
    {
      vk::UniformBufferBuilder<ChunkData> builder{};
      // clang-format off
      builder.WithLength(kMaxNumberOfChunks)
        .WithTransferDestUsage()
        .WithStorageUsage();
      // clang-format on
      chunk_data_ = builder;
      ASSERT_INITIALIZED(chunk_data_);
    }
    {
      vk::DescriptorSetUpdate update(chunk_descriptors_);
      update.AddWriteStorageBuffer(0).WithBufferInfo(chunk_data_);
    }
  });
}

ChunkRenderer::~ChunkRenderer() {
  delete tile_mesh_;
  delete chunk_data_;
  delete texture_;
}

void ChunkRenderer::RenderChunkMesh(VkCommandBuffer buffer, ChunkMesh* mesh, const uint64_t num_instances) {
  ASSERT_INITIALIZED(mesh);
  ASSERT(num_instances >= 1);
  mesh->Bind(buffer);
  vkCmdDrawIndexed(buffer, ChunkMeshClass::kTotalNumberOfIndices, num_instances, 0, 0, 0);
}

class TileMeshifier : public TileVisitor, public ChunkVisitor {
  DEFINE_NON_COPYABLE_TYPE(TileMeshifier);

 private:
  Chunk* current_chunk_ = nullptr;
  std::vector<std::vector<TileData>>& results_;

 public:
  explicit TileMeshifier(std::vector<std::vector<TileData>>& results, const uint64_t num_materials) :
    TileVisitor(),
    results_(results) {
    results_.resize(num_materials);
    for (auto idx = 0; idx < num_materials; idx++)
      results_[idx].reserve(kTotalNumberOfTilesPerChunk * (3 * 3));
  }
  ~TileMeshifier() override = default;

  auto Visit(Tile* tile) -> bool override {
    ASSERT(tile);
    ASSERT(current_chunk_);
    auto& results = results_[tile->GetMaterial()];
    auto idx = results.size();
    results.resize(idx + 1);
    results[idx] = tile->data();
    const auto& chunk_pos = current_chunk_->GetPos();
    results[idx].pos += glm::vec2(chunk_pos.x * kChunkWidth, chunk_pos.y * kChunkHeight);
    return true;
  }

  auto Visit(Chunk* chunk) -> bool override {
    ASSERT(chunk);
    current_chunk_ = chunk;
    LOG_IF(FATAL, !chunk->VisitTiles(this)) << "failed to visit tiles of: " << chunk->ToString();
    return true;
  }
};

void ChunkRenderer::RenderChunks(VkCommandBuffer buffer) {
  const auto mat_system = MaterialSystem::GetSystem();
  const auto num_materials = mat_system->GetNumberOfMaterialsLoaded();
  std::vector<std::vector<TileData>> data{};
  TileMeshifier meshifier(data, num_materials);
  const auto world = GetWorld();
  if (!world->VisitChunksAround(ChunkPos(0), 2, &meshifier))
    LOG(FATAL) << "failed to visit chunks.";
  tile_pipeline_->Bind(&buffer);
  tile_mesh_->Bind(buffer);
  for (auto idx = 0; idx < num_materials; idx++) {
    const auto& tiles = data[idx];
    if (tiles.empty())
      continue;
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