#ifndef PRT_AUDIO_WORKER_H
#define PRT_AUDIO_WORKER_H

#include <cstdint>
#include <deque>
#include <string>
#include <utility>
#include <vector>

#include "prette/audio/audio_source.h"
#include "prette/audio/audio_worker_state.h"
#include "prette/common.h"
#include "prette/os_thread.h"
#include "prette/platform.h"
#include "prette/relaxed_atomic.h"
#include "prette/uv/utils.h"

namespace prt::audio {
using AudioWorkerId = uint32_t;

class AudioWorker : OSThread {
  friend class AudioWorkerPool;
  static void OnClose(uv_async_t* handle);
  DEFINE_NON_COPYABLE_TYPE(AudioWorker);

 private:
  template <typename H>
  static inline auto Unwrap(H* handle) -> AudioWorker* {
    return uv::GetHandleData<H, AudioWorker>(handle);
  }

 public:
  static constexpr const auto kTimeout = 0;
  static constexpr const auto kRepeat = 20;  // milliseconds
  static constexpr const auto kThreadName = "AudioWorker";

 private:
  AudioWorkerId id_;
  RelaxedAtomic<AudioWorkerState> state_ = kStarting;
  std::deque<AudioSource> del_queue_{};
  uv::Loop loop_{};
  uv::Timer timer_;
  uv::Async on_close_;

  explicit AudioWorker(const AudioWorkerId id);
  void StopLoop();

  inline void SetState(const AudioWorkerState rhs) {
    state_ = rhs;
  }

  inline auto HasSourcesToDelete() const -> bool {
    return !del_queue_.empty();
  }

  inline auto NextSourceToDelete() -> AudioSource {
    const auto next = del_queue_.front();
    del_queue_.pop_front();
    return std::move(next);
  }

#define DEFINE_SET_STATE(Name)                  \
  inline void Set##Name() {                     \
    return SetState(AudioWorkerState::k##Name); \
  }
  FOR_EACH_AUDIO_WORKER_STATE(DEFINE_SET_STATE)
#undef DEFINE_SET_STATE
 protected:
  void Run() override;
  void OnTick();

 public:
  ~AudioWorker() override;

  auto GetLoop() const -> const uv::Loop& {
    return loop_;
  }

  auto GetWorkerId() const -> AudioWorkerId {
    return id_;
  }

  auto GetState() const -> AudioWorkerState {
    return (AudioWorkerState)state_;
  }

#define DEFINE_STATE_CHECK(Name)                    \
  inline auto Is##Name() const->bool {              \
    return GetState() == AudioWorkerState::k##Name; \
  }
  FOR_EACH_AUDIO_WORKER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  auto ToString() const -> std::string;
  auto Start() -> bool;
  void Shutdown();
};

class AudioWorkerPool {
 private:
  std::vector<AudioWorker*> workers_{};

 public:
  explicit AudioWorkerPool(const uint64_t num_workers);
  ~AudioWorkerPool();

  auto GetNumberOfWorkers() const -> uint64_t {
    return workers_.size();
  }

  auto GetWorkerAt(const uint64_t idx) const -> AudioWorker* {
    return workers_.at(idx);
  }

  void Shutdown();
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_WORKER_H
