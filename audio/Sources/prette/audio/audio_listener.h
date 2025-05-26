#ifndef PRT_AUDIO_LISTENER_H
#define PRT_AUDIO_LISTENER_H

#include "prette/al.h"
#include "prette/audio/audio_property.h"
#include "prette/glm.h"

namespace prt::audio {
// TODO: orientation
#define FOR_EACH_AUDIO_LISTENER_PROPERTY(V) \
  V(Gain, AL_GAIN, float)                   \
  V(Position, AL_POSITION, glm::fvec3)      \
  V(Velocity, AL_VELOCITY, glm::fvec3)

class AudioListener {
 public:
#define DEFINE_PROPERTY(Name, Id, Type)                    \
  struct Name : public MutablePropertyTemplate<Id, Type> { \
    static constexpr const auto kName = #Name;             \
  };
  FOR_EACH_AUDIO_LISTENER_PROPERTY(DEFINE_PROPERTY)
#undef DEFINE_PROPERTY
 protected:
  AudioListener();

  template <typename T>
  auto GetProperty(const PropertyId property) const -> T;

  template <typename T>
  void SetProperty(const PropertyId property, const T& value);

 public:
  virtual ~AudioListener();

  template <AudioProperty Property>
  inline auto Get() const -> typename Property::Type {
    return GetProperty<typename Property::Type>(Property::kId);
  }

  template <MutableAudioProperty Property>
  inline void Set(const typename Property::Type& rhs) {
    return SetProperty<typename Property::Type>(Property::kId, rhs);
  }

#define DEFINE_GETTER(Name, Id, Type)   \
  inline auto Get##Name() const->Type { \
    return Get<Name>();                 \
  }
  FOR_EACH_AUDIO_LISTENER_PROPERTY(DEFINE_GETTER)
#undef DEFINE_GETTER

#define DEFINE_SETTER(Name, Id, Type)      \
  inline void Set##Name(const Type& rhs) { \
    return Set<Name>(rhs);                 \
  }
  FOR_EACH_AUDIO_LISTENER_PROPERTY(DEFINE_SETTER)
#undef DEFINE_SETTER
};

#undef FOR_EACH_MUTABLE_AUDIO_LISTENER_PROPERTY
}  // namespace prt::audio

#endif  // PRT_AUDIO_LISTENER_H
