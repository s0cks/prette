#ifndef PRT_STD140_H
#define PRT_STD140_H

#include <type_traits>

namespace prt::std140 {
template <typename T>
struct is_aligned : public std::false_type {};

template <typename T>
concept IsAligned = is_aligned<T>::value;
}  // namespace prt::std140

#endif  // PRT_STD140_H
