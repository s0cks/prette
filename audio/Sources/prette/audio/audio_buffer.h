#ifndef PRT_AUDIO_BUFFER_H
#define PRT_AUDIO_BUFFER_H

#include <units.h>

#include "prette/al.h"
#include "prette/common.h"

namespace prt::audio {
class AudioBuffer {
  DEFINE_DEFAULT_COPYABLE_TYPE(AudioBuffer);

 public:
  static void Destroy(const BufferId* ids, const uint64_t num_ids);

  static inline void Destroy(const BufferId id) {
    return Destroy(&id, 1);
  }

  static inline void Destroy(const AudioBuffer& buffer) {
    return Destroy(buffer.GetId());
  }

 private:
  BufferId id_;

 public:
  AudioBuffer(const BufferId id = kInvalidBufferId) :
    id_(id) {}
  ~AudioBuffer() = default;

  auto GetId() const -> const BufferId& {
    return id_;
  }

  operator BufferId() const {
    return id_;
  }

  operator bool() const {
    return GetId() != kInvalidBufferId;
  }
};

class WavFile;
class AudioBufferBuilder {
 public:
  static constexpr const auto kDefaultFormat = AudioFormat::kMono8;
  static constexpr const units::data::byte_t kDefaultBufferSize = units::data::kilobyte_t(1);

 private:
  AudioFormat format_ = kDefaultFormat;
  uint64_t num_samples_ = 0;
  std::vector<uint8_t> data_{};

 public:
  AudioBufferBuilder() {
    data_.reserve(kDefaultBufferSize.value());
  }
  ~AudioBufferBuilder() = default;

  auto WithFormat(const AudioFormat rhs) -> AudioBufferBuilder& {
    format_ = rhs;
    return *this;
  }

  auto WithSampleRate(const uint64_t rhs) -> AudioBufferBuilder& {
    num_samples_ = rhs;
    return *this;
  }

  auto WithWavFile(const WavFile& rhs) -> AudioBufferBuilder&;
  auto WithWavFile(const fs::path path) -> AudioBufferBuilder&;
  auto WithBytes(const uint8_t* data, uint64_t num_bytes) -> AudioBufferBuilder& {
    data_.insert(std::end(data_), data, data + num_bytes);
    return *this;
  }

  inline auto WithBytes(const std::vector<uint8_t>& data) -> AudioBufferBuilder& {
    return WithBytes(data.data(), data.size());
  }

  inline auto IsValid() const -> bool {
    return num_samples_ != 0 && !data_.empty();
  }

  auto Build() -> AudioBuffer;

  inline operator AudioBuffer() {
    return Build();
  }

  inline auto operator()() -> AudioBuffer {
    return Build();
  }
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_BUFFER_H
