#ifndef PRT_AUDIO_SYSTEM_H
#define PRT_AUDIO_SYSTEM_H

#include <absl/container/flat_hash_map.h>
#include <string>
#include <unordered_map>

#include "prette/al.h"
#include "prette/audio/audio_event.h"
#include "prette/rx.h"

namespace prt::audio {
auto GetAudioEventObservable() -> AudioEventObservable;

template <typename... ArgN>
static inline auto OnAudioEvent(ArgN... args) -> rx::composite_subscription {
  return GetAudioEventObservable().subscribe(args...);
}

static constexpr const auto kDefaultGain = 0.25f;
static constexpr const auto kDefaultPos = AudioPos(0.0f);
class AudioSystem {
 public:
  static constexpr const auto kSystemName = "audio";

 private:
  ALCdevice* device_;
  ALCcontext* context_;
  std::unordered_map<std::string, AudioBuffer> sfxs_{};

  void LoadSoundEffects();

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
