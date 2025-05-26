#ifndef PRT_MIXER_H
#define PRT_MIXER_H

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
  Mixer();
  ~Mixer();
  void ResumeAll();
  void PauseAll();
  void StopAll();
};
}  // namespace prt::audio

#endif  // PRT_MIXER_H
