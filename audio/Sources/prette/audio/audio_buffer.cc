#include "prette/audio/audio_buffer.h"

#include <OpenAL/al.h>

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/audio/wav.h"
#include "prette/common.h"

namespace prt::audio {
void AudioBuffer::Destroy(const BufferId* ids, const uint64_t num_ids) {
  ASSERT(ids);
  ASSERT_GT(num_ids, 0);
  alDeleteBuffers(static_cast<ALsizei>(num_ids), ids);
  CHECK_AL_ERRORS(FATAL);
}

auto AudioBufferBuilder::WithWavFile(const WavFile& rhs) -> AudioBufferBuilder& {
  rhs.ReadBytes(data_);
  format_ = rhs.GetAudioBufferFormat();
  num_samples_ = rhs.GetSampleRate();
  return *this;
}

auto AudioBufferBuilder::WithWavFile(const fs::path path) -> AudioBufferBuilder& {
  WavFile wav(path, WavFile::kRead);
  return WithWavFile(wav);
}

auto AudioBufferBuilder::Build() -> AudioBuffer {
  BufferId id = kInvalidBufferId;
  alGenBuffers(1, &id);
  CHECK_AL_ERRORS(FATAL);
  alBufferData(id, format_, (const ALvoid*)data_.data(), (const ALsizei)data_.size(), (const ALsizei)num_samples_);
  CHECK_AL_ERRORS(FATAL);
  return id;
}
}  // namespace prt::audio