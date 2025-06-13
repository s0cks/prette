#ifndef PRT_WAV_H
#define PRT_WAV_H

#include <vector>
#include <wave.h>

#include "prette/al.h"
#include "prette/common.h"

namespace prt::audio {
class WavFile {
 public:
  enum OpenMode : uint8_t {
    kRead = WAVE_OPEN_READ,
    kWrite = WAVE_OPEN_WRITE,
  };

  enum Format : uint16_t {
    kPCM = WAVE_FORMAT_PCM,
    kIEEEFloat = WAVE_FORMAT_IEEE_FLOAT,
    kALaw = WAVE_FORMAT_ALAW,
    kMuLaw = WAVE_FORMAT_MULAW,
    kExtensible = WAVE_FORMAT_EXTENSIBLE,
    kTotalNumberOfFormats = 5,
  };

 private:
  fs::path path_{};
  WaveFile* file_ = nullptr;

 public:
  WavFile(const fs::path path, const OpenMode mode);
  ~WavFile();

  auto GetPath() const -> const fs::path& {
    return path_;
  }

  auto GetFile() const -> WaveFile* {
    return file_;
  }

  inline auto HasFile() const -> bool {
    return GetFile() != nullptr;
  }

  inline auto IsInitialized() const -> bool {
    return HasFile();
  }

  auto GetFormat() const -> Format;

  inline auto IsPCMFormat() const -> bool {
    return GetFormat() == kPCM;
  }

  inline auto IsIEEEFloatFormat() const -> bool {
    return GetFormat() == kIEEEFloat;
  }

  inline auto IsALawFormat() const -> bool {
    return GetFormat() == kALaw;
  }

  inline auto IsMuLawFormat() const -> bool {
    return GetFormat() == kMuLaw;
  }

  inline auto IsExtensibleFormat() const -> bool {
    return GetFormat() == kExtensible;
  }

  auto GetNumberOfChannels() const -> uint64_t;
  auto GetSampleRate() const -> uint64_t;
  auto GetSampleSize() const -> uint64_t;
  auto GetLength() const -> uint64_t;
  auto GetChannelMask() const -> uint64_t;
  auto GetAudioBufferFormat() const -> AudioFormat;
  auto ReadBytes(std::vector<uint8_t>& bytes) const -> bool;
};
}  // namespace prt::audio

#endif  // PRT_WAV_H
