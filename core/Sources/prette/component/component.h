#ifndef PRT_COMPONENT_H
#define PRT_COMPONENT_H

namespace prt {
static constexpr const auto kMaximumNumberOfComponents = 256;

template <typename T>
concept ComponentType = requires {
  { T::kComponentId };
  { T::kComponentName };
};

#ifdef PRT_DEBUG
struct TestComponent1 {
  static constexpr const auto kComponentId = 0;
  static constexpr const auto kComponentName = "test1";
};

struct TestComponent2 {
  static constexpr const auto kComponentId = 1;
  static constexpr const auto kComponentName = "test2";
};

struct TestComponent3 {
  static constexpr const auto kComponentId = 2;
  static constexpr const auto kComponentName = "test3";
};
#endif  // PRT_DEBUG
}  // namespace prt

#endif  // PRT_COMPONENT_H
