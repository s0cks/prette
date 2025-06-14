#include "prette/audio/audio_source.h"

#include <OpenAL/al.h>
#include <string>

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/audio/audio_buffer.h"
#include "prette/audio/audio_property.h"
#include "prette/common.h"
#include "prette/to_string.h"

namespace prt::audio {
void AudioSource::Play() const {
  alSourcePlay(GetSourceId());
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::Pause() const {
  alSourcePause(GetSourceId());
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::Stop() const {
  alSourceStop(GetSourceId());
  CHECK_AL_ERRORS(FATAL);
}

auto AudioSource::ToString() const -> std::string {
  ToStringHelper<AudioSource> helper{};
  helper.AddFieldRef("id", GetSourceId());
  return helper;
}

template <>
auto BaseAudioSource::GetProperty<float>(const PropertyId prop) const -> float {
  float value = 0.0f;
  alGetSourcef(GetSourceId(), prop, &value);
  CHECK_AL_ERRORS(FATAL);
  return value;
}

template <>
auto BaseAudioSource::GetProperty<AudioPos>(const PropertyId prop) const -> AudioPos {
  AudioPos value{};
  alGetSource3f(GetSourceId(), prop, &value[0], &value[1], &value[2]);
  CHECK_AL_ERRORS(FATAL);
  return value;
}

template <>
auto BaseAudioSource::GetProperty<bool>(const PropertyId prop) const -> bool {
  ALint value = false;
  alGetSourcei(GetSourceId(), prop, &value);
  CHECK_AL_ERRORS(FATAL);
  return value == AL_TRUE;
}

template <>
auto BaseAudioSource::GetProperty<int32_t>(const PropertyId prop) const -> int32_t {
  int32_t value = false;
  alGetSourcei(GetSourceId(), prop, &value);
  CHECK_AL_ERRORS(FATAL);
  return value;
}

template <>
void BaseAudioSource::SetProperty<float>(const PropertyId prop, const float& value) const {
  alSourcef(GetSourceId(), prop, value);
  CHECK_AL_ERRORS(FATAL);
}

template <>
void BaseAudioSource::SetProperty<AudioPos>(const PropertyId prop, const AudioPos& value) const {
  alSource3f(GetSourceId(), prop, value.x, value.y, value.z);
  CHECK_AL_ERRORS(FATAL);
}

template <>
void BaseAudioSource::SetProperty<bool>(const PropertyId prop, const bool& value) const {
  alSourcei(GetSourceId(), prop, static_cast<ALint>(value));
  CHECK_AL_ERRORS(FATAL);
}

template <>
void BaseAudioSource::SetProperty<int32_t>(const PropertyId prop, const int32_t& value) const {
  alSourcei(GetSourceId(), prop, value);
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::Destroy(const SourceId* ids, const uint64_t num_ids) {
  alDeleteSources(static_cast<ALsizei>(num_ids), ids);
  CHECK_AL_ERRORS(FATAL);
}

void AudioSource::Attach(const AudioBuffer& buffer) {
  ASSERT(buffer.GetId() != kInvalidBufferId);
  alSourcei(GetSourceId(), AL_BUFFER, static_cast<int>(buffer.GetId()));
  CHECK_AL_ERRORS(FATAL);
}

void GenSourceIds(const uint64_t num_ids, SourceId* ids) {
  alGenSources(static_cast<ALsizei>(num_ids), ids);
  CHECK_AL_ERRORS(FATAL);
}

void GenSources(const uint64_t num_sources, AudioSource* sources) {
  std::vector<SourceId> ids(num_sources);
  GenSourceIds(ids);
  for (auto idx = 0; idx < num_sources; idx++)
    sources[idx] = AudioSource(ids[idx]);
}
}  // namespace prt::audio