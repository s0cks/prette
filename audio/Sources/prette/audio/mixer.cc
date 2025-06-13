#include "prette/audio/mixer.h"

#include <algorithm>
#include <functional>
#include <mutex>

#include "prette/al.h"
#include "prette/audio/audio_source.h"
#include "prette/common.h"

namespace prt::audio {
void Mixer::VisitAll(std::function<void(const AudioSourceBuffer&)> vis) {
  mutex_.try_lock();
  std::ranges::for_each(data_, vis);
  mutex_.unlock();
}

void Mixer::DeleteSourceBuffer(const AudioSourceBuffer& rhs) {
  alDeleteSources(1, rhs.source_id_ptr());
  CHECK_AL_ERRORS(FATAL);
  alDeleteBuffers(1, rhs.buffer_id_ptr());
  CHECK_AL_ERRORS(FATAL);
}

void Mixer::CleanupStoppedSources() {
  std::lock_guard<std::mutex> guard(mutex_);
  std::erase_if(data_, [this](const AudioSourceBuffer& source_buffer) {
    if (!source_buffer.source.IsStopped())
      return false;
    DeleteSourceBuffer(source_buffer);
    return true;
  });
}
}  // namespace prt::audio