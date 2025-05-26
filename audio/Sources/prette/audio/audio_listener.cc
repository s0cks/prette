#include "prette/audio/audio_listener.h"

#include <OpenAL/al.h>
#include <utility>

#include "prette/al.h"
#include "prette/audio/audio_property.h"
#include "prette/common.h"
#include "prette/glm.h"

namespace prt::audio {
template <>
auto AudioListener::GetProperty<float>(const PropertyId prop) const -> float {
  float value = 0.0f;
  alGetListenerf(prop, &value);
  CHECK_AL_ERRORS(ERROR);
  return value;
}

template <>
auto AudioListener::GetProperty<glm::fvec3>(const PropertyId prop) const -> glm::fvec3 {
  glm::fvec3 value{};
  alGetListener3f(prop, &value[0], &value[1], &value[2]);
  CHECK_AL_ERRORS(ERROR);
  return std::move(value);
}

template <>
auto AudioListener::GetProperty<bool>(const PropertyId prop) const -> bool {
  ALint value = false;
  alGetListeneri(prop, &value);
  CHECK_AL_ERRORS(ERROR);
  return value == AL_TRUE;
}

template <>
auto AudioListener::GetProperty<int32_t>(const PropertyId prop) const -> int32_t {
  int32_t value = false;
  alGetListeneri(prop, &value);
  CHECK_AL_ERRORS(ERROR);
  return value;
}

template <>
void AudioListener::SetProperty<float>(const PropertyId prop, const float& value) {
  alListenerf(prop, value);
  CHECK_AL_ERRORS(ERROR);
}

template <>
void AudioListener::SetProperty<glm::fvec3>(const PropertyId prop, const glm::fvec3& value) {
  alListener3f(prop, value.x, value.y, value.z);
  CHECK_AL_ERRORS(ERROR);
}

template <>
void AudioListener::SetProperty<bool>(const PropertyId prop, const bool& value) {
  alListeneri(prop, static_cast<ALint>(value));
  CHECK_AL_ERRORS(ERROR);
}

template <>
void AudioListener::SetProperty<int32_t>(const PropertyId prop, const int32_t& value) {
  alListeneri(prop, value);
  CHECK_AL_ERRORS(ERROR);
}
}  // namespace prt::audio