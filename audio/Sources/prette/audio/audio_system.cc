#include "prette/audio/audio_system.h"

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/thread_local.h"

namespace prt::audio {
static ThreadLocal<AudioSystem> system_{};

AudioSystem::AudioSystem() {
  device_ = alcOpenDevice(nullptr);  // NOLINT(cppcoreguidelines-prefer-member-initializer)
  CHECK_ALC_ERRORS(device_, ERROR);
  ASSERT(device_);
  context_ = alcCreateContext(device_, nullptr);
  CHECK_ALC_ERRORS(device_, ERROR);
  ASSERT(context_);
  LOG_IF(FATAL, !alcMakeContextCurrent(context_)) << "failed to set OpenAL current context.";
  CHECK_ALC_ERRORS(device_, ERROR);

  SetGain(kDefaultGain);
  SetPos(kDefaultPos);
}

AudioSystem::~AudioSystem() = default;

void AudioSystem::SetGain(const float rhs) {
  ASSERT(rhs >= kMinGain && rhs <= kMaxGain);
  alListenerf(AL_GAIN, kDefaultGain);
  CHECK_AL_ERRORS(ERROR);
}

auto AudioSystem::GetGain() const -> float {
  float volume = kMinGain;
  alGetListenerf(AL_GAIN, &volume);
  CHECK_AL_ERRORS(ERROR);
  return volume;
}

void AudioSystem::SetPos(const AudioPos& rhs) {
  alListener3f(AL_POSITION, rhs.x, rhs.y, rhs.z);
  CHECK_AL_ERRORS(ERROR);
}

void AudioSystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new AudioSystem;
}

auto AudioSystem::GetSystem() -> AudioSystem* {
  ASSERT(IsSystemInitialized());
  return system_;
}

auto AudioSystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}
}  // namespace prt::audio