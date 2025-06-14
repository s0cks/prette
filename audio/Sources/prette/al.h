#ifndef PRT_AL_H
#define PRT_AL_H

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/glm.h"

// IWYU pragma: begin_exports
#include <OpenAL/OpenAL.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <vector>
// IWYU pragma: end_exports

namespace prt::audio {
void CheckALErrors(const google::LogSeverity severity, const char* file, const int line);
void CheckALCErrors(ALCdevice* device, const google::LogSeverity severity, const char* file, const int line);

static constexpr const auto kMinGain = 0.0f;
static constexpr const auto kMaxGain = 1.0f;

using SourceId = ALuint;
static constexpr const auto kInvalidSourceId = 0;

static inline constexpr auto IsValidSourceId(const SourceId id) -> bool {
  return id != kInvalidSourceId;
}

static inline constexpr auto IsInvalidSouceId(const SourceId id) -> bool {
  return id == kInvalidSourceId;
}

#define ASSERT_VALID_AUDIO_SOURCE_ID(Id)   ASSERT(prt::audio::IsValidSourceId((Id)))
#define ASSERT_INVALID_AUDIO_SOURCE_ID(Id) ASSERT(prt::audio::IsInvalidSourceId((Id)))

using BufferId = ALuint;
static constexpr const auto kInvalidBufferId = 0;

static inline constexpr auto IsValidBufferId(const BufferId id) -> bool {
  return id != kInvalidBufferId;
}

static inline constexpr auto IsInvalidBufferId(const BufferId id) -> bool {
  return id == kInvalidBufferId;
}

#define ASSERT_VALID_AUDIO_BUFFER_ID(Id)   ASSERT(prt::audio::IsValidBufferId((Id)))
#define ASSERT_INVALID_AUDIO_BUFFER_ID(Id) ASSERT(prt::audio::IsInvalidBufferId((Id)))

#define FOR_EACH_AUDIO_FORMAT(V)      \
  V(Mono8, 1, 8, AL_FORMAT_MONO8)     \
  V(Mono16, 1, 16, AL_FORMAT_MONO16)  \
  V(Stereo8, 2, 8, AL_FORMAT_STEREO8) \
  V(Stereo16, 2, 16, AL_FORMAT_STEREO16)

enum AudioFormat : ALenum {
  kInvalidAudioFormat = 0,
#define DEFINE_FORMAT(Name, NumChannels, BitsPerSample, Format) k##Name = (Format),
  FOR_EACH_AUDIO_FORMAT(DEFINE_FORMAT)
#undef DEFINE_FORMAT
};

using AudioPos = glm::fvec3;

auto GetAudioFormat(const uint64_t num_channels, const uint64_t bits_per_sample) -> AudioFormat;

struct AudioCone {
  float outer_gain;
  float outer_angle;
  float inner_angle;
};
}  // namespace prt::audio

#define CHECK_AL_ERRORS_AT_LEVEL(Level)          CheckALErrors(Level, __FILE__, __LINE__)
#define CHECK_AL_ERRORS(Severity)                CHECK_AL_ERRORS_AT_LEVEL(google::Severity)

#define CHECK_ALC_ERRORS_AT_LEVEL(Device, Level) CheckALCErrors(Device, Level, __FILE__, __LINE__)
#define CHECK_ALC_ERRORS(Device, Severity)       CHECK_ALC_ERRORS_AT_LEVEL(Device, google::Severity)

#endif  // PRT_AL_H
