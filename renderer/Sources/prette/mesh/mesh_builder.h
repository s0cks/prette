#ifndef PRT_MESH_BUILDER_H
#define PRT_MESH_BUILDER_H

#include <cstdint>

#include "prette/assertions.h"
#include "prette/copy_to_buffer.h"
#include "prette/index_buffer.h"
#include "prette/mesh/mesh.h"
#include "prette/mesh/mesh_class.h"
#include "prette/vertex/vertex_buffer.h"
#include "prette/vk_buffer.h"
namespace prt::vk {
template <MeshClassType M>
class BaseMeshBuilder {
 private:
 public:
  virtual ~BaseMeshBuilder() = default;
};

template <MeshClassType M>
class MeshBuilder : public BaseMeshBuilder<M> {
 public:
  using Vertex = typename M::VertexType;
  using VertexList = typename M::VertexList;

 private:
  VertexList vertices_{};

 public:
  MeshBuilder() = default;
  ~MeshBuilder() override = default;

  auto IsValid() const -> bool {
    return !vertices_.empty();
  }

  void Append(const Vertex& rhs) {
    vertices_.emplace_back(rhs);
  }

  void Append(const VertexList& rhs) {
    ASSERT_NOT_EMPTY(rhs);
    vertices_.insert(std::end(vertices_), std::begin(rhs), std::end(rhs));
  }

  auto Build() -> Mesh* {
    ASSERT(IsValid());
    vk::VertexBufferBuilder<Vertex> builder(vertices_.size());
    builder.WithTransferDestUsage();
    vk::Buffer* vertices = builder;
    ASSERT_INITIALIZED(vertices);
    return new Mesh(vertices);
  }

  auto operator()() -> Mesh* {
    return Build();
  }

  operator Mesh*() {
    return Build();
  }
};

template <IndexedMeshClassType M>
class IndexedMeshBuilder : public BaseMeshBuilder<M> {
 public:
  using Vertex = typename M::VertexType;
  using VertexList = typename M::VertexList;

  using Index = typename M::IndexType;
  using IndexList = typename M::IndexList;

 private:
  VertexList vertices_{};
  IndexList indices_{};

 public:
  IndexedMeshBuilder() :
    BaseMeshBuilder<M>() {
    vertices_.reserve(M::kTotalNumberOfVertices);
    indices_.reserve(M::kTotalNumberOfIndices);
  }
  ~IndexedMeshBuilder() override = default;

  void Append(const Vertex& rhs) {
    vertices_.emplace_back(rhs);
  }

  void Append(const Vertex* vertices, const uint64_t num_vertices) {
    ASSERT(vertices);
    ASSERT_GT(num_vertices, 0);
    vertices_.insert(std::end(vertices_), vertices, vertices + num_vertices);
  }

  void Append(const VertexList& rhs) {
    ASSERT_NOT_EMPTY(rhs);
    vertices_.insert(std::end(vertices_), std::begin(rhs), std::end(rhs));
  }

  void Append(const Index& rhs) {
    indices_.emplace_back(rhs);
  }

  void Append(const Index* indices, const uint64_t num_indices) {
    ASSERT(indices);
    ASSERT_GT(num_indices, 0);
    indices_.insert(std::end(indices_), indices, indices + num_indices);
  }

  void Append(const IndexList& rhs) {
    ASSERT_NOT_EMPTY(rhs);
    indices_.insert(std::end(indices_), std::begin(rhs), std::end(rhs));
  }

  auto IsValid() const -> bool {
    return !vertices_.empty() && !indices_.empty();
  }

  auto Build() -> IndexedMesh* {
    ASSERT(IsValid());
    vk::Buffer* vertices = nullptr;
    {
      vk::VertexBufferBuilder<Vertex> builder(vertices_.size());
      builder.WithTransferDestUsage();
      vertices = builder;
    }
    ASSERT_INITIALIZED(vertices);
    vk::CopyBytesToBufferWithStaging::Copy(&vertices_[0], vertices_.size(), vertices);

    vk::Buffer* indices = nullptr;
    {
      vk::IndexBufferBuilder<Index> builder(indices_.size());
      builder.WithTransferDestUsage();
      indices = builder;
    }
    ASSERT_INITIALIZED(indices);
    vk::CopyBytesToBufferWithStaging::Copy(&indices_[0], indices_.size(), indices);

    return new IndexedMesh(vertices, indices);
  }

  auto operator()() -> IndexedMesh* {
    return Build();
  }

  operator IndexedMesh*() {
    return Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_MESH_BUILDER_H
