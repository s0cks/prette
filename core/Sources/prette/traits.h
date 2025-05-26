#ifndef PRT_TRAITS_H
#define PRT_TRAITS_H

// IWYU pragma: begin_exports
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>
// IWYU pragma: end_exports

namespace prt {

template <class A>
struct is_fixed_array : std::false_type {};

template <class T, std::size_t N>
struct is_fixed_array<std::array<T, N>> : std::true_type {};

template <typename C>
concept FixedSizeArray = is_fixed_array<C>::value;

template <class T>
struct is_vector : std::false_type {};

template <class T, class A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template <typename C>
concept FixedSizeArrayOrVector = FixedSizeArray<C> || is_vector<C>::value;

template <typename T>
concept HasClassName = requires { T::kClassName; };

template <typename T>
concept SinglyLinkedListNodeType = requires(T value, T* next) {
  { value.GetNext() } -> std::convertible_to<std::add_pointer_t<T>>;
  { value.SetNext(next) };
};

template <typename T>
concept SinglyLinkedSortedListNodeType = requires(T value, T* next) {
  { value.GetNext() } -> std::convertible_to<std::add_pointer_t<T>>;
  { value.SetNext(next) };
  { value.GetPriority() } -> std::convertible_to<uint64_t>;
};

template <typename T>
concept AnyVkHandleType = requires(T value) {
  { value.GetHandle() };
  { value.IsInitialized() } -> std::convertible_to<bool>;
};
}  // namespace prt

#endif  // PRT_TRAITS_H
