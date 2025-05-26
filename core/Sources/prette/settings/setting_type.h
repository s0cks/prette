#ifndef PRT_SETTING_TYPE_H
#define PRT_SETTING_TYPE_H

#include <concepts>
#include <cstdint>

namespace prt {
template <typename T, typename V>
concept SettingType = requires(T value, uint8_t* value_bytes, const uint64_t num_bytes) {
  { T::kTypeName } -> std::convertible_to<const char*>;
  { T::kSettingName } -> std::convertible_to<const char*>;
  { value(value_bytes, num_bytes) } -> std::convertible_to<V>;
};

template <typename T>
struct SettingTypeTemplate {
  static constexpr const auto kValueSizeInBytes = sizeof(T);
};

struct BoolSetting : public SettingTypeTemplate<bool> {
  static constexpr const auto kTypeName = "bool";
  auto operator()(const uint8_t* bytes, const uint64_t num_bytes) -> bool;
};
}  // namespace prt

#endif  // PRT_SETTING_TYPE_H
