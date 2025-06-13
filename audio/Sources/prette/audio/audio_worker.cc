#include "prette/audio/audio_worker.h"

#include <algorithm>
#include <deque>
#include <fmt/format.h>

#include "prette/assertions.h"
#include "prette/audio/audio_source.h"
#include "prette/audio/audio_system.h"
#include "prette/audio/audio_worker_state.h"
#include "prette/common.h"
#include "prette/os_thread.h"
#include "prette/to_string.h"
#include "prette/uv/utils.h"

namespace prt::audio {
static AudioWorker* thread_ = nullptr;

AudioWorker::AudioWorker(const AudioWorkerId id) :
  OSThread(fmt::format("audio-worker-{0:d}", id)),
  id_(id),
  timer_(
      loop_,
      [this](void* data) {
        return OnTick();
      },
      kTimeout, kRepeat, this),
  on_close_(loop_, &OnClose, this) {}

AudioWorker::~AudioWorker() = default;

void AudioWorker::OnTick() {
  AudioSystem::RemoveFromPlayingIf([this](const AudioSourceBuffer& buffer) {
    if (!buffer.source.IsStopped())
      return false;
    del_queue_.push_back(buffer.source);
    return true;
  });

  DVLOG(1) << "deleting " << del_queue_.size() << " audio sources....";
  while (HasSourcesToDelete()) {
    auto next = NextSourceToDelete();
    ASSERT(next.IsStopped());
    next.DeleteSource();
  }
}

void AudioWorker::OnClose(uv_async_t* handle) {
  const auto worker = uv::GetHandleData<uv_async_t, AudioWorker>(handle);
  worker->StopLoop();
}

void AudioWorker::StopLoop() {
  SetState(kStopping);
  timer_.Stop();
  return loop_.Stop();
}

void AudioWorker::Run() {
  ASSERT(IsStarting());
  timer_.Start();
  SetState(kRunning);
  loop_.RunDefault();
  ASSERT(IsStopping());
  SetState(kStopped);
}

auto AudioWorker::ToString() const -> std::string {
  ToStringHelper<AudioWorker> helper{};
  helper.AddFieldRef("id", GetWorkerId());
  return helper;
}

AudioWorkerPool::AudioWorkerPool(const uint64_t num_workers) {
  ASSERT_GT(num_workers, 0);
  workers_.resize(num_workers);
  for (auto idx = 0; idx < num_workers; idx++) {
    workers_[idx] = new AudioWorker(static_cast<AudioWorkerId>(idx));
    ASSERT(workers_[idx]);
    if (!workers_[idx]->Start()) {
      LOG(FATAL) << "failed to start: " << workers_[idx];
      delete workers_[idx];
    }
  }
}

AudioWorkerPool::~AudioWorkerPool() {
  Shutdown();
}

auto AudioWorker::Start() -> bool {
  DVLOG(1) << "starting " << ToString() << "....";
  return OSThread::Start();
}

void AudioWorker::Shutdown() {
  DVLOG(1) << ToString() << " shutting down....";
  on_close_.Send();
  LOG_IF(FATAL, !OSThread::Join()) << "failed to join: " << ToString();
}

void AudioWorkerPool::Shutdown() {
  std::ranges::for_each(workers_, [this](AudioWorker* worker) {
    LOG_IF(FATAL, !worker->Join()) << "failed to join: " << worker->ToString();
    delete worker;
  });
}
}  // namespace prt::audio