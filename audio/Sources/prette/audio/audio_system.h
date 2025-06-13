#ifndef PRT_AUDIO_SYSTEM_H
#define PRT_AUDIO_SYSTEM_H

#include <absl/container/flat_hash_map.h>
#include <filesystem>
#include <functional>
#include <gflags/gflags.h>

#include "prette/al.h"
#include "prette/audio/audio_context.h"
#include "prette/audio/audio_device.h"
#include "prette/audio/audio_event.h"
#include "prette/audio/audio_listener.h"
#include "prette/audio/audio_source.h"
#include "prette/audio/audio_worker.h"
#include "prette/rx.h"
#include "prette/system.h"

namespace prt::audio {
DECLARE_uint64(num_audio_workers);

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

class AudioSystem {
  friend class AudioWorker;

 private:
  AudioDevice device_;
  AudioContext context_;
  AudioListener listener_{};
  AudioWorkerPool workers_;

  auto LoadAudioBufferFrom(const fs::path path) -> AudioBuffer;

  static void RemoveFromPlayingIf(std::function<bool(const AudioSourceBuffer&)> filter);
  static void AddToPlaying(const AudioSourceBuffer rhs);

 public:
  AudioSystem();
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

  auto GetWorkers() const -> const AudioWorkerPool& {
    return workers_;
  }

  void Play(const fs::path path);

  DECLARE_SYSTEM_TYPE(Audio);
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_SYSTEM_H
