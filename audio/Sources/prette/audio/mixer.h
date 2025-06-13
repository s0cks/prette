#ifndef PRT_MIXER_H
#define PRT_MIXER_H

#include <functional>
#include <mutex>

#include "prette/audio/audio_source.h"

namespace prt::audio {
class Mixer {
  friend class AudioWorkerThread;

 private:
  std::mutex mutex_{};
  AudioSourceBufferList data_{};

  void CleanupStoppedSources();
  void DeleteSourceBuffer(const AudioSourceBuffer& rhs);

 public:
  Mixer() = default;
  ~Mixer() = default;

  inline void PauseAll() {
    return VisitAll([this](const AudioSourceBuffer& buffer) {
      buffer.source.Pause();
    });
  }

  inline void ResumeAll() {
    return VisitAll([this](const AudioSourceBuffer& buffer) {
      buffer.source.Play();
    });
  }

  inline void StopAll() {
    return VisitAll([this](const AudioSourceBuffer& buffer) {
      buffer.source.Stop();
    });
  }

  void VisitAll(std::function<void(const AudioSourceBuffer&)> vis);
};
}  // namespace prt::audio

#endif  // PRT_MIXER_H
