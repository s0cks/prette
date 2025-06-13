#ifndef PRT_AUDIO_LISTENER_H
#define PRT_AUDIO_LISTENER_H

#include "prette/al.h"
#include "prette/audio/audio_property.h"
#include "prette/glm.h"

namespace prt::audio {
#define FOR_EACH_AUDIO_LISTENER_PROPERTY(V) \
  V(Gain, AL_GAIN, float)                   \
  V(Pos, AL_POSITION, glm::fvec3)           \
  V(Velocity, AL_VELOCITY, glm::fvec3)

static constexpr const auto kDefaultGain = 0.25f;
static constexpr const auto kDefaultPos = AudioPos(0.0f);
class AudioListener {
  friend class AudioSystem;

 public:
#define DEFINE_PROPERTY(Name, Id, Type)                    \
  struct Name : public MutablePropertyTemplate<Id, Type> { \
    static constexpr const auto kName = #Name;             \
  };
  FOR_EACH_AUDIO_LISTENER_PROPERTY(DEFINE_PROPERTY)
#undef DEFINE_PROPERTY
 private:
  AudioListener();

  template <typename T>
  auto GetProperty(const PropertyId property) const -> T;

  template <typename T>
  void SetProperty(const PropertyId property, const T& value);

 public:
  ~AudioListener();

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
}  // namespace prt::audio

#endif  // PRT_AUDIO_LISTENER_H
