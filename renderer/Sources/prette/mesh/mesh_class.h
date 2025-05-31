#ifndef PRT_MESH_CLASS_H
#define PRT_MESH_CLASS_H

#include <type_traits>

#include "prette/index_buffer.h"
#include "prette/index_class.h"
#include "prette/platform.h"
#include "prette/traits.h"
#include "prette/vertex/vertex.h"
#include "prette/vertex/vertex_buffer.h"

namespace prt::vk {
template <typename C>
struct is_mesh_class_t : std::false_type {};

template <typename D, VertexType V, const uint64_t NumberOfVertices, IndexType I, const uint64_t NumberOfIndices>
struct MeshClass {
  using VertexType = V;
  static constexpr const auto kTotalNumberOfVertices = NumberOfVertices;
  using VertexArray = std::array<V, kTotalNumberOfVertices>;
  using VertexBufferBuilder = vk::VertexBufferBuilder<VertexType>;

  using IndexType = I;
  using IndexClass = IndexClass<I>;
  static constexpr const auto kTotalNumberOfIndices = NumberOfIndices;
  using IndexArray = std::array<I, kTotalNumberOfIndices>;
  using IndexBufferBuilder = vk::IndexBufferBuilder<IndexType>;
};

template <typename T>
concept MeshClassType = requires {
  HasClassName<T>;
  VertexType<typename T::VertexType>;
  T::kTotalNumberOfVertices;
};

template <typename T>
concept IndexedMeshClassType = requires {
  MeshClassType<T>;
  IndexType<typename T::IndexType>;
  IndexClassType<typename T::IndexClass>;
  T::kTotalNumberOfIndices;
};
}  // namespace prt::vk

#endif  // PRT_MESH_CLASS_H
