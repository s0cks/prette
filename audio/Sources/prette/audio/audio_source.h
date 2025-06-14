#ifndef PRT_AUDIO_SOURCE_H
#define PRT_AUDIO_SOURCE_H

#include <OpenAL/al.h>
#include <functional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "prette/al.h"
#include "prette/audio/audio_buffer.h"
#include "prette/audio/audio_property.h"
#include "prette/common.h"
#include "prette/os_thread.h"  // IWYU pragma: keep

namespace prt::audio {
#define FOR_EACH_AUDIO_SOURCE_PROPERTY(V)       \
  V(Pitch, AL_PITCH, float)                     \
  V(Gain, AL_GAIN, float)                       \
  V(Position, AL_POSITION, AudioPos)            \
  V(Velocity, AL_VELOCITY, AudioPos)            \
  V(Direction, AL_DIRECTION, AudioPos)          \
  V(ConeOuterGain, AL_CONE_OUTER_GAIN, float)   \
  V(ConeInnerAngle, AL_CONE_INNER_ANGLE, float) \
  V(ConeOuterAngle, AL_CONE_OUTER_ANGLE, float) \
  V(Looping, AL_LOOPING, bool)

class BaseAudioSource {
 public:
#define DEFINE_PROPERTY(Name, Id, Type)             \
  struct Name : public PropertyTemplate<Id, Type> { \
    static constexpr const auto kName = #Name;      \
  };
  FOR_EACH_AUDIO_SOURCE_PROPERTY(DEFINE_PROPERTY)
#undef DEFINE_PROPERTY

  struct MinGain : public FloatPropertyTemplate<AL_MIN_GAIN> {
    static constexpr const auto kName = "MinGain";
  };
  struct MaxGain : public FloatPropertyTemplate<AL_MAX_GAIN> {
    static constexpr const auto kName = "MaxGain";
  };
  struct RolloffFactor : public FloatPropertyTemplate<AL_ROLLOFF_FACTOR> {
    static constexpr const auto kName = "RolloffFactor";
  };
  struct ReferenceDistance : public FloatPropertyTemplate<AL_REFERENCE_DISTANCE> {
    static constexpr const auto kName = "ReferenceDistance";
  };
  struct SourceRelative : public BoolPropertyTemplate<AL_SOURCE_RELATIVE> {
    static constexpr const auto kName = "SourceRelative";
  };
  struct SourceState : public IntPropertyTemplate<AL_SOURCE_STATE> {
    static constexpr const auto kName = "SourceState";
  };
  struct SourceType : public IntPropertyTemplate<AL_SOURCE_TYPE> {
    static constexpr const auto kName = "SourceType";
  };
  struct Buffer : public IntPropertyTemplate<AL_BUFFER> {
    static constexpr const auto kName = "Buffer";
  };
  struct NumberOfBuffersQueued : public IntPropertyTemplate<AL_BUFFERS_QUEUED> {
    static constexpr const auto kName = "NumberOfBuffersQueued";
  };
  struct NumberOfBuffersProcessed : public IntPropertyTemplate<AL_BUFFERS_PROCESSED> {
    static constexpr const auto kName = "NumberOfBuffersProcessed";
  };
  struct SecondsOffset : public FloatPropertyTemplate<AL_SEC_OFFSET> {
    static constexpr const auto kName = "SecondsOffset";
  };
  struct SampleOffset : public FloatPropertyTemplate<AL_SAMPLE_OFFSET> {
    static constexpr const auto kName = "SampleOffset";
  };
  struct ByteOffset : public FloatPropertyTemplate<AL_BYTE_OFFSET> {
    static constexpr const auto kName = "ByteOffset";
  };

 private:
  SourceId id_;

 protected:
  explicit BaseAudioSource(const SourceId id) :
    id_(id) {}

  template <typename T>
  auto GetProperty(const PropertyId property) const -> T;

  template <typename T>
  void SetProperty(const PropertyId property, const T& value) const;

  template <AudioProperty Property>
  inline auto Get() const -> typename Property::Type {
    return GetProperty<typename Property::Type>(Property::kId);
  }

  template <AudioProperty Property>
  inline void Set(const typename Property::Type& rhs) const {
    return SetProperty<typename Property::Type>(Property::kId, rhs);
  }

  void SetId(const SourceId id) {
    ASSERT_VALID_AUDIO_SOURCE_ID(id);
    id_ = id;
  }

 public:
  ~BaseAudioSource() = default;

  auto GetSourceId() const -> const SourceId& {
    return id_;
  }

  inline void SetPitch(const float rhs) {
    return Set<Pitch>(rhs);
  }

  inline auto GetPitch() const -> float {
    return Get<Pitch>();
  }

  inline void SetGain(const float rhs) {
    return Set<Gain>(rhs);
  }

  inline auto GetGain() const -> float {
    return Get<Gain>();
  }

  inline void SetPos(const AudioPos rhs) {
    return Set<Position>(rhs);
  }

  inline void SetPos(const float x, const float y, const float z) {
    return SetPos(AudioPos(x, y, z));
  }

  inline void SetPos(const float xyz) {
    return SetPos(AudioPos(xyz));
  }

  inline auto GetPos() const -> AudioPos {
    return Get<Position>();
  }

  inline void SetLooping(const bool rhs = true) {
    return Set<Looping>(rhs);
  }

  inline void ClearLooping() {
    return SetLooping(false);
  }

  inline auto GetMinGain() const -> float {
    return Get<MinGain>();
  }

  inline auto GetMaxGain() const -> float {
    return Get<MaxGain>();
  }

  inline auto GetRolloffFactor() const -> float {
    return Get<RolloffFactor>();
  }

  inline auto GetReferenceDistance() const -> float {
    return Get<ReferenceDistance>();
  }

  inline auto IsSourceRelative() const -> bool {
    return Get<SourceRelative>();
  }

  inline auto GetSourceState() const -> int {
    return Get<SourceState>();
  }

  inline auto IsPlaying() const -> bool {
    return GetSourceState() == AL_PLAYING;
  }

  inline auto IsStopped() const -> bool {
    return GetSourceState() == AL_STOPPED;
  }

  inline auto GetSourceType() const -> int {
    return Get<SourceType>();
  }

  inline auto GetBuffer() const -> int {
    return Get<Buffer>();
  }

  inline auto GetNumberOfBuffersQueued() const -> int {
    return Get<NumberOfBuffersQueued>();
  }

  inline auto GetNumberOfBuffersProcessed() const -> int {
    return Get<NumberOfBuffersProcessed>();
  }

  inline auto GetSampleOffset() const -> float {
    return Get<SampleOffset>();
  }

  inline auto GetSecondsOffset() const -> float {
    return Get<SecondsOffset>();
  }

  inline auto GetByteOffset() const -> float {
    return Get<ByteOffset>();
  }

  auto GetAudioCone() const -> AudioCone {
    return {
        .outer_gain = Get<ConeOuterGain>(),
        .outer_angle = Get<ConeOuterAngle>(),
        .inner_angle = Get<ConeInnerAngle>(),
    };
  }
};

class AudioSource : public BaseAudioSource {
  friend class AudioWorker;
  DEFINE_DEFAULT_COPYABLE_TYPE(AudioSource);

 public:
  static void Destroy(const SourceId* ids, const uint64_t num_ids);

  static inline void Destroy(SourceId id) {
    return Destroy(&id, 1);
  }

  static inline void Destroy(const AudioSource& source) {
    DVLOG(1) << "destroying: " << source;
    return Destroy(source.GetSourceId());
  }

 private:
  void DeleteSource();

 public:
  explicit AudioSource(const SourceId id = kInvalidAudioFormat) :
    BaseAudioSource(id) {}
  ~AudioSource() = default;

  void Play() const;
  void Pause() const;
  void Stop() const;
  void Attach(const AudioBuffer& buffer);
  auto GetCone() const -> AudioCone;
  auto ToString() const -> std::string;

  operator SourceId() const {
    return GetSourceId();
  }

  operator bool() const {
    return GetSourceId() != kInvalidSourceId;
  }

  friend auto operator<<(std::ostream& stream, const AudioSource& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};

void GenSourceIds(const uint64_t num_ids, SourceId* ids);

static inline void GenSourceIds(std::vector<SourceId>& ids) {
  return GenSourceIds(ids.size(), ids.data());
}

static inline void GenSourceIds(const uint64_t num_ids, std::vector<SourceId>& ids) {
  ids.resize(num_ids);
  return GenSourceIds(ids);
}

void GenSources(const uint64_t num_sources, AudioSource* sources);

using AudioSourcePredicate = std::function<bool(const AudioSource&)>;

struct AudioSourceBuffer {
  AudioSource source;
  AudioBuffer buffer;

  AudioSourceBuffer(AudioSource src, AudioBuffer buff) :
    source(std::move(src)),
    buffer(std::move(buff)) {
    source.Attach(buffer);
  }

  inline auto source_id_ptr() const -> const SourceId* {
    return &source.GetSourceId();
  }

  inline auto buffer_id_ptr() const -> const BufferId* {
    return &buffer.GetId();
  }

  static inline void Destroy(const AudioSourceBuffer& buffer) {
    AudioSource::Destroy(buffer.source);
    AudioBuffer::Destroy(buffer.buffer);
  }
};

using AudioSourceBufferList = std::vector<AudioSourceBuffer>;
}  // namespace prt::audio

#endif  // PRT_AUDIO_SOURCE_H
