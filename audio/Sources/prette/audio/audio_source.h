#ifndef PRT_AUDIO_SOURCE_H
#define PRT_AUDIO_SOURCE_H

#include <OpenAL/al.h>
#include <functional>
#include <vector>

#include "prette/al.h"
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

class AudioSource {
  DEFINE_DEFAULT_COPYABLE_TYPE(AudioSource);

 public:
#define DEFINE_PROPERTY(Name, Id, Type)                    \
  struct Name : public MutablePropertyTemplate<Id, Type> { \
    static constexpr const auto kName = #Name;             \
  };
  FOR_EACH_AUDIO_SOURCE_PROPERTY(DEFINE_PROPERTY)
#undef DEFINE_PROPERTY

  struct MinGain : public ImmutableFloatPropertyTemplate<AL_MIN_GAIN> {
    static constexpr const auto kName = "MinGain";
  };
  struct MaxGain : public ImmutableFloatPropertyTemplate<AL_MAX_GAIN> {
    static constexpr const auto kName = "MaxGain";
  };
  struct RolloffFactor : public ImmutableFloatPropertyTemplate<AL_ROLLOFF_FACTOR> {
    static constexpr const auto kName = "RolloffFactor";
  };
  struct ReferenceDistance : public ImmutableFloatPropertyTemplate<AL_REFERENCE_DISTANCE> {
    static constexpr const auto kName = "ReferenceDistance";
  };
  struct SourceRelative : public ImmutableBoolPropertyTemplate<AL_SOURCE_RELATIVE> {
    static constexpr const auto kName = "SourceRelative";
  };
  struct SourceState : public ImmutableIntPropertyTemplate<AL_SOURCE_STATE> {
    static constexpr const auto kName = "SourceState";
  };
  struct SourceType : public ImmutableIntPropertyTemplate<AL_SOURCE_TYPE> {
    static constexpr const auto kName = "SourceType";
  };
  struct Buffer : public ImmutableIntPropertyTemplate<AL_BUFFER> {
    static constexpr const auto kName = "Buffer";
  };
  struct NumberOfBuffersQueued : public ImmutableIntPropertyTemplate<AL_BUFFERS_QUEUED> {
    static constexpr const auto kName = "NumberOfBuffersQueued";
  };
  struct NumberOfBuffersProcessed : public ImmutableIntPropertyTemplate<AL_BUFFERS_PROCESSED> {
    static constexpr const auto kName = "NumberOfBuffersProcessed";
  };
  struct SecondsOffset : public ImmutableFloatPropertyTemplate<AL_SEC_OFFSET> {
    static constexpr const auto kName = "SecondsOffset";
  };
  struct SampleOffset : public ImmutableFloatPropertyTemplate<AL_SAMPLE_OFFSET> {
    static constexpr const auto kName = "SampleOffset";
  };
  struct ByteOffset : public ImmutableFloatPropertyTemplate<AL_BYTE_OFFSET> {
    static constexpr const auto kName = "ByteOffset";
  };

 private:
  SourceId id_ = 0;

  AudioSource();

  template <typename T>
  auto GetProperty(const PropertyId property) const -> T;

  template <typename T>
  void SetProperty(const PropertyId property, const T& value) const;

  void DeleteSource();

 public:
  AudioSource(const SourceId id) :
    id_(id) {}
  ~AudioSource() = default;

  auto GetId() const -> const SourceId& {
    return id_;
  }

  void Play() const;
  void Pause() const;
  void Stop() const;

  auto GetCone() const -> AudioCone;

  template <AudioProperty Property>
  auto Get() const -> typename Property::Type {
    return GetProperty<typename Property::Type>(Property::kId);
  }

  template <MutableAudioProperty Property>
  void Set(const typename Property::Type& rhs) const {
    return SetProperty<typename Property::Type>(Property::kId, rhs);
  }

  auto GetMinGain() const -> float {
    return Get<MinGain>();
  }

  auto GetMaxGain() const -> float {
    return Get<MaxGain>();
  }

  auto GetRolloffFactor() const -> float {
    return Get<RolloffFactor>();
  }

  auto GetReferenceDistance() const -> float {
    return Get<ReferenceDistance>();
  }

  auto IsSourceRelative() const -> bool {
    return Get<SourceRelative>();
  }

  auto GetSourceState() const -> int {
    return Get<SourceState>();
  }

  auto IsPlaying() const -> bool {
    return GetSourceState() == AL_PLAYING;
  }

  auto IsStopped() const -> bool {
    return GetSourceState() == AL_STOPPED;
  }

  auto GetSourceType() const -> int {
    return Get<SourceType>();
  }

  auto GetBuffer() const -> int {
    return Get<Buffer>();
  }

  auto GetNumberOfBuffersQueued() const -> int {
    return Get<NumberOfBuffersQueued>();
  }

  auto GetNumberOfBuffersProcessed() const -> int {
    return Get<NumberOfBuffersProcessed>();
  }

  auto GetSampleOffset() const -> float {
    return Get<SampleOffset>();
  }

  auto GetSecondsOffset() const -> float {
    return Get<SecondsOffset>();
  }

  auto GetByteOffset() const -> float {
    return Get<ByteOffset>();
  }

  operator SourceId() const {
    return GetId();
  }
};

using AudioSourcePredicate = std::function<bool(const AudioSource&)>;

struct AudioSourceBuffer {
  AudioSource source;
  BufferId buffer;

  inline auto source_id_ptr() const -> const SourceId* {
    return &source.GetId();
  }

  inline auto buffer_id_ptr() const -> const BufferId* {
    return &buffer;
  }
};

using AudioSourceBufferList = std::vector<AudioSourceBuffer>;
}  // namespace prt::audio

#endif  // PRT_AUDIO_SOURCE_H
