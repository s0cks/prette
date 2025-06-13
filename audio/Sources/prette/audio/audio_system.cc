#include "prette/audio/audio_system.h"

#include <filesystem>
#include <functional>
#include <marl/scheduler.h>
#include <mutex>
#include <utility>
#include <vector>

#include "prette/al.h"
#include "prette/assertions.h"
#include "prette/audio/audio_source.h"
#include "prette/audio/wav.h"
#include "prette/common.h"
#include "prette/event.h"
#include "prette/flags.h"
#include "prette/system.h"
#include "prette/thread_local.h"  // IWYU pragma: keep

namespace prt::audio {
DEFINE_uint64(num_audio_workers, 1, "Number of audio system worker threads.");

static std::mutex sfx_mutex_{};
static std::vector<AudioSourceBuffer> sfx_{};

DEFINE_THREAD_LOCAL_SYSTEM_TYPE(Audio);

DEFINE_GLOBAL_EVENT_SUBJECT(AudioEvent, events);

AudioSystem::AudioSystem() :
  device_(),
  context_(device_),
  workers_(FLAGS_num_audio_workers) {}

AudioSystem::~AudioSystem() = default;

void AudioSystem::AddToPlaying(const AudioSourceBuffer rhs) {
  std::lock_guard<std::mutex> guard(sfx_mutex_);
  sfx_.emplace_back(std::move(rhs));
}

void AudioSystem::RemoveFromPlayingIf(std::function<bool(const AudioSourceBuffer&)> filter) {
  std::lock_guard<std::mutex> guard(sfx_mutex_);
  std::erase_if(sfx_, filter);
}

auto AudioSystem::LoadAudioBufferFrom(const fs::path path) -> AudioBuffer {
  WavFile wav(path, WavFile::kRead);
  ASSERT_INITIALIZED((&wav));
  std::vector<uint8_t> bytes{};
  LOG_IF(FATAL, !wav.ReadBytes(bytes)) << "failed to read wav bytes from: " << path;
  DLOG(INFO) << "read " << bytes.size() << "b from: " << path;
  return AudioBuffer(wav.GetAudioBufferFormat(), bytes, wav.GetSampleRate());
}

void AudioSystem::Play(const fs::path path) {
  ASSERT(fs::exists(path) && fs::is_regular_file(path));
  AudioSource source{};
  GenSources(1, &source);
  source.Set<AudioSource::Pitch>(1.0f);
  source.Set<AudioSource::Gain>(1.0f);
  source.Set<AudioSource::Position>(glm::vec3(0.0f));
  source.Set<AudioSource::Looping>(false);

  const auto buffer = LoadAudioBufferFrom(path);
  source.Attach(buffer);

  source.Play();
  AddToPlaying(AudioSourceBuffer{
      .source = source,
      .buffer = buffer,
  });
}
}  // namespace prt::audio