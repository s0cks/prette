#ifndef PRT_PIPELINE_H
#define PRT_PIPELINE_H

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/gfx.h"
#include "prette/pipeline/pipeline_builder.h"
#include "prette/pipeline/pipeline_cache.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/relaxed_atomic.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
static inline void InitPipelineViewportState(VkPipelineViewportStateCreateInfo& create_info,
                                             const std::vector<VkViewport>& viewports,
                                             const std::vector<VkRect2D>& scissors) {
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  create_info.viewportCount = viewports.size();
  create_info.pViewports = viewports.data();
  create_info.scissorCount = scissors.size();
  create_info.pScissors = scissors.data();
}

class RenderPipeline;
using RenderPipelinePredicate = std::function<bool(RenderPipeline*)>;

class RenderPipeline : public vk::NamedHandleTemplate<VkPipeline> {
 public:
  static constexpr const VkExtent2D kDefaultExtent = {0, 0};

 protected:
  vk::PipelineLayout* layout_;
  vk::PipelineCache* cache_;
  VkExtent2D extent_;

 public:
  RenderPipeline(std::string name, const VkExtent2D& extent, const VkGraphicsPipelineCreateInfo& create_info,
                 vk::PipelineLayout* layout, vk::PipelineCache* cache);
  ~RenderPipeline() override;

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  inline auto HasExtent() const -> bool {
    return GetExtent() != kDefaultExtent;
  }

  auto GetPipelineLayout() const -> vk::PipelineLayout* {
    return layout_;
  }

  auto GetPipelineCache() const -> vk::PipelineCache* {
    return cache_;
  }

  auto ToString() const -> std::string override;
  void Bind(VkCommandBuffer* buffer, const VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

  operator VkPipeline() const {
    return GetHandle();
  }

 public:
  static auto FromJson(const std::string name) -> RenderPipeline*;
};

template <typename Vertex, const uint64_t MaxNumberOfVertices, typename Index, const uint64_t MaxNumberOfIndices>
class RenderPipelineTemplate : public RenderPipeline {
 public:
  static constexpr const auto kVertexSize = sizeof(Vertex);
  static constexpr const auto kMaxNumberOfVertices = MaxNumberOfVertices;
  static constexpr const auto kTotalVertexBufferSize = kVertexSize * kMaxNumberOfVertices;

  static constexpr const auto kIndexSize = sizeof(Index);
  static constexpr const auto kMaxNumberOfIndices = MaxNumberOfIndices;
  static constexpr const auto kTotalIndexBufferSize = kIndexSize * kMaxNumberOfIndices;

  using VertexList = std::vector<Vertex>;
  using IndexList = std::vector<Index>;

 private:
  VertexList vertices_{};
  vk::Buffer* vbuffer_ = nullptr;
  RelaxedAtomic<bool> vertices_changed_ = false;

  IndexList indices_{};
  vk::Buffer* ibuffer_ = nullptr;
  RelaxedAtomic<bool> indices_changed_ = false;

 protected:
  RenderPipelineTemplate(std::string name, const VkExtent2D& extent, const VkGraphicsPipelineCreateInfo& create_info,
                         vk::PipelineLayout* layout, vk::PipelineCache* cache) :
    RenderPipeline(name, extent, create_info, layout, cache) {
    {
      vk::BufferBuilder builder{};
      // clang-format off
      vbuffer_ = builder.WithSize(kTotalVertexBufferSize)
        .WithVertexBufferUsage()
        .Build();
      // clang-format on
      ASSERT_INITIALIZED(vbuffer_);
    }
    {
      vk::BufferBuilder builder{};
      // clang-format off
      ibuffer_ = builder.WithSize(kTotalIndexBufferSize)
        .WithIndexBufferUsage()
        .Build();
      // clang-format on
      ASSERT_INITIALIZED(ibuffer_);
    }
  }

  void UpdateVertexBuffer(Vertex* data, const uint64_t num_vertices, const bool staging = true) {
    ASSERT_INITIALIZED(vbuffer_);
    ASSERT(data);
    ASSERT(num_vertices >= 1 && num_vertices <= kMaxNumberOfVertices);
    const auto total_size = sizeof(Vertex) * num_vertices;
    if (staging) {
      vk::CopyBytesToBufferWithStaging copy(data, total_size);
      copy(GetVertexBuffer());
    } else {
      vk::CopyBytesToBuffer copy(data, total_size);
      copy(GetVertexBuffer());
    }
    vertices_changed_ = false;
  }

  void UpdateVertexBuffer(const VertexList& data, const bool staging = true) {
    ASSERT_INITIALIZED(vbuffer_);
    ASSERT(!data.empty());
    return UpdateVertexBuffer(data.data(), data.size(), staging);
  }

  void UpdateVertexBuffer(const bool staging = true) {
    return UpdateVertexBuffer(vertices_.data(), vertices_.size(), staging);
  }

  auto AppendVertex(const Vertex& rhs) -> Index {
    const auto index = GetNumberOfVertices();
    vertices_changed_ = true;
    vertices_.emplace_back(rhs);
    return index;
  }

  auto AppendVertices(const VertexList& rhs) -> Index {
    const auto start = GetNumberOfVertices();
    vertices_changed_ = true;
    vertices_.insert(std::end(vertices_), std::begin(rhs), std::end(rhs));
    return start;
  }

  void UpdateIndexBuffer(Index* data, const uint64_t num_indices, const bool staging = true) {
    ASSERT_INITIALIZED(ibuffer_);
    ASSERT(data);
    ASSERT(num_indices >= 1 && num_indices <= kMaxNumberOfIndices);
    const auto total_size = sizeof(Index) * num_indices;
    if (staging) {
      vk::CopyBytesToBufferWithStaging copy(data, total_size);
      copy(GetIndexBuffer());
    } else {
      vk::CopyBytesToBuffer copy(data, total_size);
      copy(GetIndexBuffer());
    }
    indices_changed_ = false;
  }

  void UpdateIndexBuffer(const IndexList& data, const bool staging = true) {
    ASSERT_INITIALIZED(ibuffer_);
    ASSERT(!data.empty());
    return UpdateIndexBuffer(data.data(), data.size(), staging);
  }

  void UpdateIndexBuffer(const bool staging = true) {
    return UpdateIndexBuffer(indices_.data(), indices_.size(), staging);
  }

  void AppendIndex(const Index& rhs) {
    indices_changed_ = true;
    indices_.emplace_back(rhs);
  }

  void AppendIndices(const IndexList& rhs) {
    indices_changed_ = true;
    indices_.insert(std::end(indices_), std::begin(rhs), std::end(rhs));
  }

 public:
  ~RenderPipelineTemplate() override {
    delete vbuffer_;
    delete ibuffer_;
  }

  void Update() {
    if (HasVerticesChanged())
      UpdateVertexBuffer();
    if (HasIndicesChanged())
      UpdateIndexBuffer();
  }

  auto GetVertexBuffer() const -> vk::Buffer* {
    return vbuffer_;
  }

  inline auto HasVertexBuffer() const -> bool {
    return vk::IsInitialized(GetVertexBuffer());
  }

  auto HasVerticesChanged() const -> bool {
    return (bool)vertices_changed_;
  }

  auto GetNumberOfVertices() const -> uint64_t {
    return vertices_.size();
  }

  auto GetIndexBuffer() const -> vk::Buffer* {
    return ibuffer_;
  }

  inline auto HasIndexBuffer() const -> bool {
    return vk::IsInitialized(GetIndexBuffer());
  }

  inline auto IsInitialized() const -> bool {
    return vk::IsInitialized(GetVertexBuffer()) && vk::IsInitialized(GetIndexBuffer());
  }

  auto HasIndicesChanged() const -> bool {
    return (bool)indices_changed_;
  }

  auto GetNumberOfIndices() const -> uint64_t {
    return indices_.size();
  }
};

class RenderPipeline;
class RenderPipelineBuilder : public TemplateRenderPipelineBuilder<RenderPipeline> {
 public:
  explicit RenderPipelineBuilder(const VkExtent2D extent = {}) :
    TemplateRenderPipelineBuilder<RenderPipeline>(extent) {}
  ~RenderPipelineBuilder() override = default;

  auto Build() -> RenderPipeline* override;

  operator VkGraphicsPipelineCreateInfo() const {
    return pipeline_;
  }
};

class RenderPipelineVisitor {
 protected:
  RenderPipelineVisitor() = default;

 public:
  virtual ~RenderPipelineVisitor() = default;
  virtual auto Visit(RenderPipeline* rhs) -> bool = 0;
};

auto VisitAllRenderPipelines(RenderPipelineVisitor* vis) -> bool;
auto VisitAllRenderPipelines(RenderPipelinePredicate vis) -> bool;
auto GetNumberOfRenderPipelines() -> uint64_t;

class RenderPipelineFinalizer : public RenderPipelineVisitor {
 private:
  uint64_t num_finalized_ = 0;

 public:
  RenderPipelineFinalizer() = default;
  ~RenderPipelineFinalizer() override = default;
  auto Visit(RenderPipeline* rhs) -> bool override;

  auto GetNumberOfObjectsFinalized() const -> uint64_t {
    return num_finalized_;
  }

  auto operator()(RenderPipeline* rhs) -> bool {
    return Visit(rhs);
  }

 public:
  static void FinalizeAll();
  static void FinalizeAll(const std::vector<RenderPipeline*>& rhs);
};
}  // namespace prt::vk

#endif  // PRT_PIPELINE_H
