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
};

class SoundEffect {
 private:
  SoundEffectData data_;
  AudioSource source_;
  AudioBuffer buffer_;

 public:
  SoundEffect();
  ~SoundEffect();
};
}  // namespace prt::audio

#endif  // PRT_SOUND_EFFECT_H
