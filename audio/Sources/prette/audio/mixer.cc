#include "prette/audio/mixer.h"

#include <algorithm>

#include "prette/al.h"
#include "prette/audio/audio_source.h"
#include "prette/common.h"

namespace prt::audio {
Mixer::Mixer() = default;
Mixer::~Mixer() = default;

void Mixer::PauseAll() {
  mutex_.try_lock();
  std::ranges::for_each(data_, [this](const AudioSourceBuffer& buffer) {
    buffer.source.Pause();
  });
  mutex_.unlock();
}

void Mixer::ResumeAll() {
  mutex_.try_lock();
  std::ranges::for_each(data_, [this](const AudioSourceBuffer& buffer) {
    buffer.source.Play();
  });
  mutex_.unlock();
}

void Mixer::StopAll() {
  mutex_.try_lock();
  std::ranges::for_each(data_, [this](const AudioSourceBuffer& buffer) {
    buffer.source.Stop();
  });
  mutex_.unlock();
}

void Mixer::DeleteSourceBuffer(const AudioSourceBuffer& rhs) {
  alDeleteSources(1, rhs.source_id_ptr());
  CHECK_AL_ERRORS(FATAL);
  alDeleteBuffers(1, rhs.buffer_id_ptr());
  CHECK_AL_ERRORS(FATAL);
}

void Mixer::CleanupStoppedSources() {
  mutex_.try_lock();
  std::erase_if(data_, [this](const AudioSourceBuffer& source_buffer) {
    if (!source_buffer.source.IsStopped())
      return false;
    DeleteSourceBuffer(source_buffer);
    return true;
  });
  mutex_.unlock();
}
}  // namespace prt::audio