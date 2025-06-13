#ifndef PRT_AUDIO_CONTEXT_H
#define PRT_AUDIO_CONTEXT_H

#include "prette/al.h"
#include "prette/audio/audio_device.h"
#include "prette/common.h"

namespace prt::audio {
class AudioContext {
  friend class AudioSystem;
  DEFINE_NON_COPYABLE_TYPE(AudioContext);

 public:
  using Handle = ALCcontext;

 private:
  Handle* handle_ = nullptr;

  explicit AudioContext(const AudioDevice& device);

 public:
  ~AudioContext();

  auto GetHandle() const -> Handle* {
    return handle_;
  }

  inline auto HasHandle() const -> bool {
    return GetHandle() != nullptr;
  }

  inline auto IsInitialized() const -> bool {
    return HasHandle();
  }
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_CONTEXT_H
