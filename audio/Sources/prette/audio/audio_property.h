#ifndef PRT_AUDIO_PROPERTY_H
#define PRT_AUDIO_PROPERTY_H

#include <concepts>
#include <cstdint>

#include "prette/al.h"
#include "prette/glm.h"

namespace prt::audio {
#define FOR_EACH_AUDIO_PROPERTY_TYPE(V) \
  V(Int, int32_t)                       \
  V(Bool, bool)                         \
  V(Float, float)                       \
  V(Vec3, glm::fvec3)

using PropertyId = ALenum;

template <const PropertyId Id, typename T, const bool Mutable>
struct PropertyTemplate {
  static constexpr const auto kId = Id;
  static constexpr const auto kMutable = Mutable;
  using Type = T;
};

template <const PropertyId Id, typename T>
struct MutablePropertyTemplate : public PropertyTemplate<Id, T, true> {};

#define DEFINE_MUTABLE_PROPERTY_TEMPLATE(Name, Type) \
  template <const PropertyId Id>                     \
  struct Mutable##Name##PropertyTemplate : public MutablePropertyTemplate<Id, Type> {};
FOR_EACH_AUDIO_PROPERTY_TYPE(DEFINE_MUTABLE_PROPERTY_TEMPLATE)
#undef DEFINE_MUTABLE_PROPERTY_TYPE

template <const PropertyId Id, typename T>
struct ImmutablePropertyTemplate : public PropertyTemplate<Id, T, false> {};

#define DEFINE_IMMUTABLE_PROPERTY_TEMPLATE(Name, Type) \
  template <const PropertyId Id>                       \
  struct Immutable##Name##PropertyTemplate : public ImmutablePropertyTemplate<Id, Type> {};
FOR_EACH_AUDIO_PROPERTY_TYPE(DEFINE_IMMUTABLE_PROPERTY_TEMPLATE)
#undef DEFINE_IMMUTABLE_PROPERTY_TEMPLATE

template <typename T>
concept AudioProperty = requires(T) {
  { T::kName } -> std::convertible_to<const char*>;
  { T::kId } -> std::convertible_to<PropertyId>;
  typename T::Type;
};

template <typename T>
concept MutableAudioProperty = requires(T) {
  AudioProperty<T>;
  { T::kMutable };
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_PROPERTY_H
