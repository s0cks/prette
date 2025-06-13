#include "prette/audio/audio_device.h"

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/common.h"

namespace prt::audio {
AudioDevice::AudioDevice() {
  handle_ = alcOpenDevice(nullptr);  // NOLINT(cppcoreguidelines-prefer-member-initializer)
  CHECK_ALC_ERRORS(handle_, ERROR);
  ASSERT_INITIALIZED(this);
}

AudioDevice::~AudioDevice() {
  ASSERT_INITIALIZED(this);
  alcCloseDevice(handle_);
}
}  // namespace prt::audio