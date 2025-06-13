#include "prette/audio/audio_context.h"

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/audio/audio_device.h"
#include "prette/common.h"

namespace prt::audio {
AudioContext::AudioContext(const AudioDevice& device) {
  handle_ = alcCreateContext(device, nullptr);  // NOLINT(cppcoreguidelines-prefer-member-initializer)
  CHECK_ALC_ERRORS(device, ERROR);
  ASSERT_INITIALIZED(this);
  LOG_IF(FATAL, !alcMakeContextCurrent(handle_)) << "failed to set OpenAL current context.";
  CHECK_ALC_ERRORS(device, ERROR);
}

AudioContext::~AudioContext() {
  alcDestroyContext(handle_);
  ASSERT_UNINITIALIZED(this);
}
}  // namespace prt::audio