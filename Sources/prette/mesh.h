#ifndef PRT_MESH_H
#define PRT_MESH_H

#include <array>
#include <type_traits>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/index_buffer.h"
#include "prette/index_class.h"
#include "prette/mesh_class.h"
#include "prette/platform.h"
#include "prette/uniform_buffer.h"
#include "prette/vertex.h"
#include "prette/vertex_buffer.h"
#include "prette/vk.h"

namespace prt::vk {
template <typename D>
struct is_mesh_data_t : std::false_type {};

template <typename T>
concept MeshDataType = is_mesh_data_t<T>::value;

template <MeshDataType Data, VertexType Vertex, const uint64_t NumberOfVertices, IndexType Index,
          const uint64_t NumberOfIndices>
class Mesh {
 public:
  using MeshType = Mesh<Data, Vertex, NumberOfVertices, Index, NumberOfIndices>;
  using DataType = Data;
  using VertexType = Vertex;
  using VertexBufferType = VertexBuffer<VertexType, NumberOfVertices>;
  using VertexArrayType = std::array<Vertex, NumberOfVertices>;
  using IndexType = Index;
  using IndexBufferType = IndexBuffer<IndexType, NumberOfIndices>;
  using IndexArrayType = std::array<Index, NumberOfIndices>;
  static constexpr const auto kTotalNumberOfVertices = NumberOfVertices;
  static constexpr const auto kTotalNumberOfIndices = NumberOfIndices;

 private:
  DataType data_{};

 public:
  Mesh(const DataType data) :
    data_(std::move(data)) {}
  virtual ~Mesh() = default;
  virtual auto GetVertexBuffer() const -> VertexBufferType* = 0;
  virtual auto GetIndexBuffer() const -> IndexBufferType* = 0;

  inline auto HasVertexBuffer() const -> bool {
    return vk::IsInitialized(GetVertexBuffer());
  }

  inline auto HasIndexBuffer() const -> bool {
    return vk::IsInitialized(GetIndexBuffer());
  }
};

template <MeshDataType Data, VertexType Vertex, const uint64_t NumberOfVertices, IndexType Index,
          const uint64_t NumberOfIndices>
class SingleMesh : public Mesh<Data, Vertex, NumberOfVertices, Index, NumberOfIndices> {
 public:
  using MeshType = Mesh<Data, Vertex, NumberOfVertices, Index, NumberOfIndices>;
  using VertexBuffer = MeshType::VertexBufferType;
  using IndexBuffer = MeshType::IndexBufferType;

 private:
  VertexBuffer* vertex_buffer_ = nullptr;
  IndexBuffer* index_buffer_ = nullptr;

 public:
  SingleMesh();
  ~SingleMesh() override = default;

  auto GetVertexBuffer() const -> VertexBuffer* override {
    return vertex_buffer_;
  }

  auto GetIndexBuffer() const -> IndexBuffer* override {
    return index_buffer_;
  }
};

template <typename M>
struct is_mesh_t : std::false_type {};

template <typename M>
concept MeshType = is_mesh_t<M>::value;

template <IndexedMeshClassType M, const uint64_t MaxNumberOfInstances>
class InstancedMesh {
 public:
  using InstanceDataType = typename M::DataType;
  using InstanceList = std::vector<InstanceDataType>;

  using VertexType = typename M::VertexType;
  static constexpr const auto kNumberOfVerticesPerInstance = M::kTotalNumberOfVertices;
  using VertexBufferType = vk::VertexBuffer<VertexType, kNumberOfVerticesPerInstance>;
  using VertexList = std::vector<VertexType>;
  using VertexArray = std::array<VertexType, kNumberOfVerticesPerInstance>;

  using IndexType = typename M::IndexType;
  static constexpr const auto kNumberOfIndicesPerInstance = M::kTotalNumberOfIndices;
  using IndexBufferType = vk::IndexBuffer<IndexType, kNumberOfIndicesPerInstance>;
  using IndexList = std::vector<IndexType>;
  using IndexArray = std::array<IndexType, kNumberOfIndicesPerInstance>;

  static constexpr const auto kMaxNumberOfInstances = MaxNumberOfInstances;

  using InstanceBufferType = vk::UniformBuffer<InstanceDataType, kMaxNumberOfInstances>;

 private:
  InstanceBufferType* instances_ = nullptr;
  VertexBufferType* vertices_ = nullptr;
  IndexBufferType* indices_ = nullptr;

 public:
  explicit InstancedMesh(const uint64_t num_instances) :
    instances_(new InstanceBufferType(num_instances)),
    vertices_(new VertexBufferType(num_instances * kNumberOfVerticesPerInstance)),
    indices_(new IndexBufferType(num_instances * kNumberOfIndicesPerInstance)) {}
  explicit InstancedMesh(const uint64_t num_instances, const VertexArray& vertices, const IndexArray& indices) :
    InstancedMesh(num_instances) {
    SetVertices(vertices);
    SetIndices(indices);
  }
  ~InstancedMesh() {
    delete instances_;
    delete vertices_;
    delete indices_;
  }

  auto GetInstanceBuffer() const -> InstanceBufferType* {
    return instances_;
  }

  inline auto HasInstanceBuffer() const -> bool {
    return vk::IsInitialized(GetInstanceBuffer());
  }

  inline auto GetNumberOfInstances() const -> uint64_t {
    ASSERT(HasInstanceBuffer());
    return GetInstanceBuffer()->GetSize();
  }

  auto GetVertexBuffer() const -> VertexBufferType* {
    return vertices_;
  }

  inline auto HasVertexBuffer() const -> bool {
    return vk::IsInitialized(GetVertexBuffer());
  }

  auto GetIndexBuffer() const -> IndexBufferType* {
    return indices_;
  }

  inline auto HasIndexBuffer() const -> bool {
    return vk::IsInitialized(GetIndexBuffer());
  }

  void Append(M* rhs) {
    ASSERT(rhs);
  }

  void SetVertices(const VertexArray& rhs) {
    return GetVertexBuffer()->SetData(rhs);
  }

  void SetIndices(const IndexArray& rhs) {
    return GetIndexBuffer()->SetData(rhs);
  }

  template <FixedSizeArrayOrVector C>
  void SetInstances(const C& rhs) {
    return GetInstanceBuffer()->SetData(rhs);
  }

  auto IsDirty() const -> bool {
    return GetInstanceBuffer()->IsDirty() || GetVertexBuffer()->IsDirty() || GetIndexBuffer()->IsDirty();
  }

  void Sync(const bool staging = false) {
    if (GetInstanceBuffer()->IsDirty())
      GetInstanceBuffer()->Sync(staging);
    if (GetVertexBuffer()->IsDirty())
      GetVertexBuffer()->Sync(staging);
    if (GetIndexBuffer()->IsDirty())
      GetIndexBuffer()->Sync(staging);
  }

  void Draw(VkCommandBuffer buffer, uint64_t num_instances = VK_WHOLE_SIZE) {
    if (num_instances == VK_WHOLE_SIZE)
      num_instances = instances_->GetSize();
    std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(buffer, 0, 1, &GetVertexBuffer()->GetBuffer(), offsets.data());
    vkCmdBindIndexBuffer(buffer, GetIndexBuffer()->GetBuffer(), 0, M::IndexClass::kFormat);
    vkCmdDrawIndexed(buffer, kNumberOfIndicesPerInstance, num_instances, 0, 0, 0);
    GetInstanceBuffer()->UnmarkDirty();
    GetVertexBuffer()->UnmarkDirty();
    GetIndexBuffer()->UnmarkDirty();
  }

  auto IsInitialized() const -> bool {
    return vk::AllInitialized(GetVertexBuffer(), GetIndexBuffer(), GetInstanceBuffer());
  }
};
}  // namespace prt::vk

#endif  // PRT_MESH_H
