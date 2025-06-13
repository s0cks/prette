#include "prette/audio/wav.h"

#include <cstdint>
#include <filesystem>
#include <utility>
#include <vector>
#include <wave.h>

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/common.h"

namespace prt::audio {
WavFile::WavFile(const fs::path path, const OpenMode mode) :
  path_(std::move(path)),
  file_(wave_open(path_.c_str(), mode)) {
  ASSERT_INITIALIZED(this);
}

WavFile::~WavFile() {
  if (file_)
    wave_close(file_);
}

auto WavFile::GetFormat() const -> Format {
  ASSERT_INITIALIZED(this);
  return static_cast<Format>(wave_get_format(file_));
}

auto WavFile::GetNumberOfChannels() const -> uint64_t {
  ASSERT_INITIALIZED(this);
  return wave_get_num_channels(GetFile());
}

auto WavFile::GetSampleRate() const -> uint64_t {
  ASSERT_INITIALIZED(this);
  return wave_get_sample_rate(GetFile());
}

auto WavFile::GetSampleSize() const -> uint64_t {
  ASSERT_INITIALIZED(this);
  return wave_get_sample_size(GetFile());
}

auto WavFile::GetLength() const -> uint64_t {
  ASSERT_INITIALIZED(this);
  return wave_get_length(GetFile());
}

auto WavFile::GetChannelMask() const -> uint64_t {
  ASSERT_INITIALIZED(this);
  return wave_get_channel_mask(GetFile());
}

auto WavFile::GetAudioBufferFormat() const -> AudioFormat {
  return GetAudioFormat(GetNumberOfChannels(), GetSampleSize() * kBitsPerByte);
}

auto WavFile::ReadBytes(std::vector<uint8_t>& bytes) const -> bool {
  ASSERT_INITIALIZED(this);
  const auto sample_size = wave_get_sample_size(GetFile());
  const auto num_channels = wave_get_num_channels(GetFile());
  const auto length = wave_get_length(GetFile());
  const auto total_size = sample_size * num_channels * length;
  bytes.resize(total_size);
  return wave_read(GetFile(), &bytes[0], length) == length;
}
}  // namespace prt::audio