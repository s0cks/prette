#ifndef PRT_AUDIO_DEVICE_H
#define PRT_AUDIO_DEVICE_H

#include <OpenAL/alc.h>

#include "prette/common.h"

namespace prt::audio {
class AudioDevice {
  DEFINE_NON_COPYABLE_TYPE(AudioDevice);

 public:
  using Handle = ALCdevice;

 private:
  Handle* handle_ = nullptr;

 public:
  AudioDevice();
  ~AudioDevice();

  auto Get() const -> Handle* {
    return handle_;
  }

  inline auto HasHandle() const -> bool {
    return Get() != nullptr;
  }

  inline auto IsInitialized() const -> bool {
    return HasHandle();
  }

  operator Handle*() const {
    return Get();
  }
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_DEVICE_H
