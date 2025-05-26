#ifndef PRT_INDEX_CLASS_H
#define PRT_INDEX_CLASS_H

#include <type_traits>
#include <vulkan/vulkan_core.h>

#include "prette/platform.h"  // IWYU pragma: keep
#include "prette/vk.h"

namespace prt::vk {
#define FOR_EACH_INDEX_CLASS(V)       \
  V(uint8_t, VK_INDEX_TYPE_UINT8_KHR) \
  V(uint16_t, VK_INDEX_TYPE_UINT16)   \
  V(uint32_t, VK_INDEX_TYPE_UINT32)

template <typename T>
struct is_index_t : std::false_type {};

#define DEFINE_IS_INDEX_TYPE(Type, Format) \
  template <>                              \
  struct is_index_t<Type> : std::true_type {};
FOR_EACH_INDEX_CLASS(DEFINE_IS_INDEX_TYPE)
#undef DEFINE_IS_INDEX_TYPE

template <typename T>
concept IndexType = is_index_t<T>::value;

template <IndexType Type>
struct IndexClass {
  static constexpr const auto kIndexSize = sizeof(Type);
};

template <typename T>
struct is_index_class_t : std::false_type {};

#define DEFINE_INDEX_CLASS(Type, Format)            \
  template <>                                       \
  struct IndexClass<Type> {                         \
    static constexpr const auto kClassName = #Type; \
    static constexpr const auto kFormat = Format;   \
  };                                                \
  template <>                                       \
  struct is_index_class_t<IndexClass<Type>> : std::true_type {};
FOR_EACH_INDEX_CLASS(DEFINE_INDEX_CLASS);
#undef DEFINE_INDEX_CLASS

template <typename T>
concept IndexClassType = is_index_class_t<T>::value && requires { T::kClassName; };
}  // namespace prt::vk

#endif  // PRT_INDEX_CLASS_H
