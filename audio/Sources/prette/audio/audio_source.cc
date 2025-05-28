#include "prette/audio/audio_source.h"

#include <OpenAL/al.h>
#include <utility>

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/audio/audio_property.h"
#include "prette/common.h"

namespace prt::audio {
void AudioSource::Play() const {
  alSourcePlay(GetId());
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::Pause() const {
  alSourcePause(GetId());
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::Stop() const {
  alSourceStop(GetId());
  CHECK_AL_ERRORS(FATAL);
}

template <>
auto AudioSource::GetProperty<float>(const PropertyId prop) const -> float {
  float value = 0.0f;
  alGetSourcef(GetId(), prop, &value);
  CHECK_AL_ERRORS(ERROR);
  return value;
}

template <>
auto AudioSource::GetProperty<AudioPos>(const PropertyId prop) const -> AudioPos {
  AudioPos value{};
  alGetSource3f(GetId(), prop, &value[0], &value[1], &value[2]);
  CHECK_AL_ERRORS(ERROR);
  return std::move(value);
}

template <>
auto AudioSource::GetProperty<bool>(const PropertyId prop) const -> bool {
  ALint value = false;
  alGetSourcei(GetId(), prop, &value);
  CHECK_AL_ERRORS(ERROR);
  return value == AL_TRUE;
}

template <>
auto AudioSource::GetProperty<int32_t>(const PropertyId prop) const -> int32_t {
  int32_t value = false;
  alGetSourcei(GetId(), prop, &value);
  CHECK_AL_ERRORS(ERROR);
  return value;
}

template <>
void AudioSource::SetProperty<float>(const PropertyId prop, const float& value) const {
  alSourcef(GetId(), prop, value);
  CHECK_AL_ERRORS(ERROR);
}

template <>
void AudioSource::SetProperty<AudioPos>(const PropertyId prop, const AudioPos& value) const {
  alSource3f(GetId(), prop, value.x, value.y, value.z);
  CHECK_AL_ERRORS(ERROR);
}

template <>
void AudioSource::SetProperty<bool>(const PropertyId prop, const bool& value) const {
  alSourcei(GetId(), prop, static_cast<ALint>(value));
  CHECK_AL_ERRORS(ERROR);
}

template <>
void AudioSource::SetProperty<int32_t>(const PropertyId prop, const int32_t& value) const {
  alSourcei(GetId(), prop, value);
  CHECK_AL_ERRORS(ERROR);
}

AudioSource::AudioSource() {
  alGenSources(1, &id_);
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::DeleteSource() {
  alDeleteSources(1, &id_);
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::Attach(const AudioBuffer& buffer) {
  ASSERT(buffer.GetId() != kInvalidBufferId);
  alSourcei(GetId(), AL_BUFFER, static_cast<int>(buffer.GetId()));
  CHECK_AL_ERRORS(FATAL);
}

auto AudioSource::GetCone() const -> AudioCone {
  return AudioCone{
      .outer_gain = Get<ConeOuterGain>(),
      .outer_angle = Get<ConeOuterAngle>(),
      .inner_angle = Get<ConeInnerAngle>(),
  };
}
}  // namespace prt::audio