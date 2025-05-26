#ifndef PRT_AUDIO_SYSTEM_H
#define PRT_AUDIO_SYSTEM_H

#include "prette/al.h"

namespace prt::audio {
static constexpr const auto kDefaultGain = 0.25f;
static constexpr const auto kDefaultPos = AudioPos(0.0f);
class AudioSystem {
 public:
  static constexpr const auto kSystemName = "audio";

 private:
  ALCdevice* device_;
  ALCcontext* context_;

 public:
  AudioSystem();
  ~AudioSystem();

  void SetGain(const float rhs);
  auto GetGain() const -> float;

  void SetPos(const AudioPos& rhs);

 public:
  static void InitSystem();
  static auto GetSystem() -> AudioSystem*;
  static auto IsSystemInitialized() -> bool;
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_SYSTEM_H
