#ifndef PRT_VERTEX_H
#define PRT_VERTEX_H

#include <type_traits>

#include "prette/platform.h"
#include "prette/std140.h"  // IWYU pragma: keep

namespace prt {
template <typename T>
struct is_vertex : std::false_type {};

template <typename T>
concept VertexType = is_vertex<T>::value;

template <typename T>
struct is_2d_vertex_t : is_vertex<T> {};

template <typename T>
concept VertexType2D = is_2d_vertex_t<T>::value;
}  // namespace prt

#define DEFINE_VERTEX_ATTR(Index, Name, Type, Format, Alignment) Alignment Type Name{};

#define _DEFINE_ATTRIBUTE_DESCRIPTION(Index, Name, Type, Format, Alignment) \
  attributes.at(Index).binding = 0;                                         \
  attributes.at(Index).location = Index;                                    \
  attributes.at(Index).format = Format;                                     \
  attributes.at(Index).offset = offsetof(Vertex, Name);

#define DEFINE_VERTEX_GET_ATTR_DESCRIPTIONS(ForEachAttr, TotalNumberOfVertices)                      \
  using AttrDescriptionArray = std::array<VkVertexInputAttributeDescription, TotalNumberOfVertices>; \
  static auto GetAttributeDescriptions() -> AttrDescriptionArray {                                   \
    AttrDescriptionArray attributes{};                                                               \
    ForEachAttr(_DEFINE_ATTRIBUTE_DESCRIPTION);                                                      \
    return attributes;                                                                               \
  }

#define _DEFINE_VERTEX_BINDING_DESCRIPTION(Name, Index, Type, Rate) \
  Name.binding = Index;                                             \
  Name.stride = sizeof(Type);                                       \
  Name.inputRate = Rate;

#define _DEFINE_VERTEX_GET_BINDING_DESCRIPTION(Type, Rate)                        \
  static inline auto GetBindingDescription() -> VkVertexInputBindingDescription { \
    VkVertexInputBindingDescription binding{};                                    \
    _DEFINE_VERTEX_BINDING_DESCRIPTION(binding, 0, Type, Rate);                   \
    return binding;                                                               \
  }

#define DEFINE_VERTEX_GET_INSTANCE_BINDING_DESCRIPTION(Type) \
  _DEFINE_VERTEX_GET_BINDING_DESCRIPTION(Type, VK_VERTEX_INPUT_RATE_INSTANCE);

#define DEFINE_VERTEX_GET_BINDING_DESCRIPTION(Type) \
  _DEFINE_VERTEX_GET_BINDING_DESCRIPTION(Type, VK_VERTEX_INPUT_RATE_VERTEX)

#define DECLARE_VERTEX_CLASS(Name, ForEachAttr, NumberOfAttrs)            \
  namespace Name {                                                        \
  struct Vertex {                                                         \
    static constexpr const auto kVertexClassName = #Name;                 \
    static constexpr const auto kTotalNumberOfAttributes = NumberOfAttrs; \
    ForEachAttr(DEFINE_VERTEX_ATTR);                                      \
    auto operator==(const Vertex& rhs) const -> bool;                     \
    auto operator!=(const Vertex& rhs) const -> bool;                     \
    DEFINE_VERTEX_GET_BINDING_DESCRIPTION(Vertex);                        \
    DEFINE_VERTEX_GET_ATTR_DESCRIPTIONS(ForEachAttr, NumberOfAttrs);      \
  };                                                                      \
  }                                                                       \
  template <>                                                             \
  struct is_vertex<Name::Vertex> : std::true_type {};

#endif  // PRT_VERTEX_H
