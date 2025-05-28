#include "prette/al.h"

#include <ostream>

#include "prette/common.h"

namespace prt::audio {
#define FOR_EACH_AL_ERROR(V) \
  V(AL_INVALID_NAME)         \
  V(AL_INVALID_ENUM)         \
  V(AL_INVALID_OPERATION)    \
  V(AL_INVALID_VALUE)        \
  V(AL_OUT_OF_MEMORY)

class ALStatus {
 private:
  ALenum value_;

 public:
  ALStatus(const ALenum value) :
    value_(value) {}
  ~ALStatus() = default;

  auto value() const -> ALenum {
    return value_;
  }

  auto IsOk() const -> bool {
    return value() == AL_NO_ERROR;
  }

  operator ALenum() const {
    return value();
  }

  operator bool() const {
    return IsOk();
  }

  friend auto operator<<(std::ostream& stream, const ALStatus& rhs) -> std::ostream& {
    if (rhs)
      return stream << "ok.";
    switch (rhs.value()) {
#define DEFINE_TOSTRING(Name) \
  case Name:                  \
    return stream << #Name;
      FOR_EACH_AL_ERROR(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default:
        return stream << "unknown ALStatus: " << static_cast<int64_t>(rhs.value());
    }
  }
};

#define FOR_EACH_ALC_ERROR(V) \
  V(ALC_INVALID_DEVICE)       \
  V(ALC_INVALID_CONTEXT)      \
  V(ALC_INVALID_ENUM)         \
  V(ALC_INVALID_VALUE)        \
  V(ALC_OUT_OF_MEMORY)

class ALCStatus {
 private:
  ALCenum value_;

 public:
  ALCStatus(const ALCenum value) :
    value_(value) {}
  ~ALCStatus() = default;

  auto value() const -> ALCenum {
    return value_;
  }

  auto IsOk() const -> bool {
    return value() == ALC_NO_ERROR;
  }

  operator ALCenum() const {
    return value();
  }

  operator bool() const {
    return IsOk();
  }

  friend auto operator<<(std::ostream& stream, const ALCStatus& rhs) -> std::ostream& {
    if (rhs)
      return stream << "ok.";
    switch (rhs.value()) {
#define DEFINE_TOSTRING(Name) \
  case Name:                  \
    return stream << #Name;
      FOR_EACH_ALC_ERROR(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
      default:
        return stream << "unknown ALCStatus: " << static_cast<int64_t>(rhs.value());
    }
  }
};

AudioBuffer::AudioBuffer(const AudioFormat format, const uint8_t* bytes, const uint64_t num_bytes,
                         const uint64_t num_samples) :
  id_(kInvalidBufferId) {
  alGenBuffers(1, &id_);
  CHECK_AL_ERRORS(FATAL);
  alBufferData(id_, format, (const ALvoid*)bytes, (const ALsizei)num_bytes, (const ALsizei)num_samples);
  CHECK_AL_ERRORS(FATAL);
}

AudioBuffer::~AudioBuffer() {
  alDeleteBuffers(1, &id_);
}

void CheckALErrors(const google::LogSeverity severity, const char* file, const int line) {
  ALStatus status = AL_NO_ERROR;
  while (!(status = alGetError()))
    google::LogMessage(file, line, severity).stream() << "encountered: " << status;
}

void CheckALCErrors(ALCdevice* device, const google::LogSeverity severity, const char* file, const int line) {
  ALCStatus status = AL_NO_ERROR;
  while (!(status = alcGetError(device)))
    google::LogMessage(file, line, severity).stream() << "encountered: " << status;
}

static inline auto IsValidNumberOfChannels(const uint64_t num_channels) -> bool {
  return num_channels <= 2 && num_channels >= 1;
}

static inline auto IsValidBitsPerSample(const uint64_t bits_per_sample) -> bool {
  return bits_per_sample == 8 || bits_per_sample == 16;
}

auto GetAudioFormat(const uint64_t num_channels, const uint64_t bits_per_sample) -> AudioFormat {
  if (!IsValidNumberOfChannels(num_channels)) {
    LOG(ERROR) << "invalid AudioFormat: num_channels=" << num_channels;
    return kInvalidAudioFormat;
  } else if (!IsValidBitsPerSample(bits_per_sample)) {
    LOG(ERROR) << "invalid AudioFormat: invalid bits_per_sample=" << bits_per_sample;
    return kInvalidAudioFormat;
  }
#define DEFINE_CHECK_FORMAT(Name, NumChannels, BitsPerSample, Format)             \
  else if (num_channels == (NumChannels) && bits_per_sample == (BitsPerSample)) { \
    return AudioFormat::k##Name;                                                  \
  }
  FOR_EACH_AUDIO_FORMAT(DEFINE_CHECK_FORMAT)
#undef DEFINE_CHECK_FORMAT

  LOG(ERROR) << "invalid AudioFormat: num_channels=" << num_channels << ", bits_per_sample=" << bits_per_sample;
  return kInvalidAudioFormat;
}
}  // namespace prt::audio