#ifndef PRT_AUDIO_PROPERTY_H
#define PRT_AUDIO_PROPERTY_H

#include <concepts>
#include <cstdint>

#include "prette/al.h"

namespace prt::audio {
#define FOR_EACH_AUDIO_PROPERTY_TYPE(V) \
  V(Int, int32_t)                       \
  V(Bool, bool)                         \
  V(Float, float)                       \
  V(Vec3, glm::fvec3)

using PropertyId = ALenum;

template <const PropertyId Id, typename T>
struct PropertyTemplate {
  static constexpr const auto kId = Id;
  using Type = T;
};

#define DECLARE_AUDIO_PROPERTY_TEMPLATE_TYPE(Name, Type) \
  template <const PropertyId Id>                         \
  struct Name##PropertyTemplate : public PropertyTemplate<Id, Type> {};
FOR_EACH_AUDIO_PROPERTY_TYPE(DECLARE_AUDIO_PROPERTY_TEMPLATE_TYPE)
#undef DECLARE_AUDIO_PROPERTY_TEMPLATE_TYPE

template <typename T>
concept AudioProperty = requires(T) {
  { T::kName } -> std::convertible_to<const char*>;
  { T::kId } -> std::convertible_to<PropertyId>;
  typename T::Type;
};

}  // namespace prt::audio

#endif  // PRT_AUDIO_PROPERTY_H
