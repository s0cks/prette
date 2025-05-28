#include "prette/audio/audio_system.h"

#include <filesystem>

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/event.h"
#include "prette/flags.h"
#include "prette/thread_local.h"

namespace prt::audio {
static ThreadLocal<AudioSystem> system_{};
DEFINE_GLOBAL_EVENT_SUBJECT(AudioEvent, events);

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
  LoadSoundEffects();
}

AudioSystem::~AudioSystem() = default;

void AudioSystem::LoadSoundEffects() {
  const auto root = fs::path(FLAGS_resources) / "sfx";
  DVLOG(1) << "loading sound effects from " << root << "....";
  for (const auto& dir_entry : fs::directory_iterator(root)) {}
}

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