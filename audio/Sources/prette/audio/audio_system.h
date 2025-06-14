#ifndef PRT_AUDIO_SYSTEM_H
#define PRT_AUDIO_SYSTEM_H

#include <absl/container/flat_hash_map.h>
#include <filesystem>
#include <gflags/gflags.h>

#include "prette/al.h"
#include "prette/audio/audio_buffer.h"
#include "prette/audio/audio_context.h"
#include "prette/audio/audio_device.h"
#include "prette/audio/audio_event.h"
#include "prette/audio/audio_listener.h"
#include "prette/audio/audio_source.h"
#include "prette/common.h"
#include "prette/rx.h"
#include "prette/system.h"
#include "prette/uv/check.h"
#include "prette/uv/idle.h"
#include "prette/uv/loop.h"

namespace prt::audio {
auto GetAudioEventObservable() -> AudioEventObservable;

template <typename... ArgN>
static inline auto OnAudioEvent(ArgN... args) -> rx::composite_subscription {
  return GetAudioEventObservable().subscribe(args...);
}

#define ON_EVENT(Name)                                                                   \
  static inline auto Get##Name##EventObservable()->Name##EventObservable {               \
    return GetAudioEventObservable().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                                      \
  template <typename... ArgN>                                                            \
  static inline auto On##Name##Event(ArgN... args)->r::composite_subscription {          \
    return GetName##EventObservable().subscribe(args...);                                \
  }
FOR_EACH_AUDIO_EVENT(ON_EVENT)
#undef ON_EVENT

static inline constexpr auto normalize(const float value) -> float {
  return value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
}

class AudioSystem {
  friend class AudioWorker;

 public:
  static constexpr const auto kDefaultPollTimeout = 0;
  static constexpr const auto kDefaultPollRepeat = 30;

 private:
  uv::Idle idle_;
  uv::Check check_;
  AudioDevice device_;
  AudioContext context_;
  AudioListener listener_{};
  std::vector<AudioSourceBuffer> playing_{};
  std::vector<AudioSourceBuffer> stopped_{};

  void OnTick();
  void OnCheck();

 public:
  AudioSystem(uv::Loop& loop);
  ~AudioSystem();

  auto GetDevice() const -> const AudioDevice& {
    return device_;
  }

  auto GetContext() const -> const AudioContext& {
    return context_;
  }

  auto GetListener() const -> const AudioListener& {
    return listener_;
  }

  inline auto GetVolume() const -> float {
    return GetListener().GetGain();
  }

  inline void SetVolume(const float rhs) {
    return listener_.Set<AudioListener::Gain>(rhs);
  }

  void Play(const AudioSource& source, const AudioBuffer& buffer);
  void Play(const AudioSource& source, const fs::path path);
  void Play(const fs::path path);

  DECLARE_SYSTEM_TYPE(Audio);
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_SYSTEM_H
