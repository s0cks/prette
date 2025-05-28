#ifndef PRT_AUDIO_WORKER_THREAD_H
#define PRT_AUDIO_WORKER_THREAD_H

#include <cstdint>

#include "prette/os_thread.h"
#include "prette/platform.h"
#include "prette/relaxed_atomic.h"

namespace prt::audio {
using AudioWorkerId = uint32_t;

#define FOR_EACH_AUDIO_WORKER_THREAD_STATE(V) \
  V(Starting)                                 \
  V(Running)                                  \
  V(Paused)                                   \
  V(Stopping)                                 \
  V(Stopped)                                  \
  V(Error)

// TODO: use libuv event loop
class AudioWorkerThread {
  static void HandleThread(void* data);

 public:
  static constexpr const auto kThreadName = "AudioWorkerThread";

  enum State {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_AUDIO_WORKER_THREAD_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };

 private:
  AudioWorkerId id_;
  RelaxedAtomic<State> state_ = kStopped;
  ThreadId thid_{};
  pthread_mutex_t mixer_mutex_{};
  RelaxedAtomic<bool> should_terminate_ = false;

  auto Start() -> bool;

  inline void SetState(const State rhs) {
    state_ = rhs;
  }

  inline void SetShouldTerminate(const bool rhs) {
    should_terminate_ = rhs;
  }

  inline auto ShouldTerminate() const -> bool {
    return (bool)should_terminate_;
  }

 public:
  AudioWorkerThread(const AudioWorkerId id);
  ~AudioWorkerThread();

  auto GetWorkerId() const -> AudioWorkerId {
    return id_;
  }

  auto GetState() const -> State {
    return (State)state_;
  }

#define DEFINE_STATE_CHECK(Name)         \
  inline auto Is##Name() const->bool {   \
    return GetState() == State::k##Name; \
  }
  FOR_EACH_AUDIO_WORKER_THREAD_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  auto Join() -> bool;

 public:
  static void Init();
  static auto GetAudioWorkerThread() -> AudioWorkerThread*;
  static void Shutdown();
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_WORKER_THREAD_H
