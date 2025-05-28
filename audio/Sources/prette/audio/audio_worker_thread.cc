#include "prette/audio/audio_worker_thread.h"

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/os_thread.h"

namespace prt::audio {
static AudioWorkerThread* thread_ = nullptr;

AudioWorkerThread::AudioWorkerThread(const AudioWorkerId id) :
  id_(id) {
  pthread_mutex_init(&mixer_mutex_, nullptr);
  LOG_IF(FATAL, !Start()) << "failed to start AudioWorkerThread";
}

AudioWorkerThread::~AudioWorkerThread() {
  LOG_IF(FATAL, !Join()) << "failed to shutdown AudioWorkerThread";
}

void AudioWorkerThread::HandleThread(void* data) {
  const auto worker_thread = (AudioWorkerThread*)data;  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  ASSERT(worker_thread->IsStopped());
  DLOG(INFO) << "starting AudioWorkerThread #" << worker_thread->GetWorkerId() << "....";
  worker_thread->SetState(kStarting);
  // do something?
  worker_thread->SetState(kRunning);
  worker_thread->SetShouldTerminate(false);
  while (!worker_thread->ShouldTerminate()) {}
  worker_thread->SetState(kStopping);
  // do something
  worker_thread->SetState(kStopped);
}

auto AudioWorkerThread::Join() -> bool {
  return prt::Join(thid_);
}

auto AudioWorkerThread::Start() -> bool {
  return prt::Start(&thid_, kThreadName, &HandleThread, this);
}

void AudioWorkerThread::Init() {
  // TODO: check calling thread is main thread
  ASSERT(thread_ == nullptr);
  thread_ = new AudioWorkerThread(1);
}

auto AudioWorkerThread::GetAudioWorkerThread() -> AudioWorkerThread* {
  ASSERT(thread_);
  return thread_;
}

void AudioWorkerThread::Shutdown() {
  // TODO: check calling thread is main thread
  delete thread_;
}
}  // namespace prt::audio