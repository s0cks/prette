#ifndef PRT_COMPONENT_H
#define PRT_COMPONENT_H

namespace prt {
template <typename T>
concept ComponentType = requires {
  { T::kComponentId };
  { T::kComponentName };
};

struct TestComponent {
  static constexpr const auto kComponentId = 0;
  static constexpr const auto kComponentName = "test";
};
}  // namespace prt

#endif  // PRT_COMPONENT_H
