#ifndef PRT_SOUND_EFFECT_H
#define PRT_SOUND_EFFECT_H

#include <string>

#include "prette/al.h"
#include "prette/audio/audio_source.h"

namespace prt::audio {
static constexpr const auto kDefaultSoundEffectPitch = 1.0f;
static constexpr const auto kDefaultSoundEffectGain = 1.0f;
static constexpr const AudioPos kDefaultSoundEffectPos = {0.0f, 0.0f, 0.0f};
struct SoundEffectData {
  std::string name{};
  AudioFormat format{};
  float pitch = kDefaultSoundEffectPitch;
  float gain = kDefaultSoundEffectGain;
  AudioPos pos = kDefaultSoundEffectPos;
  bool looped = false;

  void ApplyTo(const AudioSource& source) {
    source.Set<AudioSource::Pitch>(pitch);
    source.Set<AudioSource::Gain>(gain);
    source.Set<AudioSource::Position>(pos);
    source.Set<AudioSource::Looping>(looped);
  }

  auto operator>>(const AudioSource& source) -> const AudioSource& {
    ApplyTo(source);
    return source;
  }
};
}  // namespace prt::audio

#endif  // PRT_SOUND_EFFECT_H
