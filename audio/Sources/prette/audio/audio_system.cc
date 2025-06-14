#include "prette/audio/audio_system.h"

#include <algorithm>
#include <filesystem>
#include <marl/scheduler.h>

#include "prette/assertions.h"
#include "prette/audio/audio_buffer.h"
#include "prette/audio/audio_source.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/event.h"
#include "prette/thread_local.h"  // IWYU pragma: keep
#include "prette/uv/loop.h"

namespace prt::audio {
DEFINE_GLOBAL_EVENT_SUBJECT(AudioEvent, events);
static ThreadLocal<AudioSystem> system_{};

void AudioSystem::InitSystem() {
  DLOG(INFO) << "initializing AudioSystem....";
  const auto engine = GetEngine();
  ASSERT(!IsSystemInitialized());
  system_ = new AudioSystem(engine->GetLoop());
}

auto AudioSystem::GetSystem() -> AudioSystem* {
  ASSERT(IsSystemInitialized());
  return system_;
}

auto AudioSystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}

AudioSystem::AudioSystem(uv::Loop& loop) :
  device_(),
  context_(device_),
  idle_(loop,
        [this]() {
          return OnTick();
        }),
  check_(loop, [this]() {
    return OnCheck();
  }) {
  idle_.Start();
  check_.Start();
}

AudioSystem::~AudioSystem() {
  idle_.Stop();
  idle_.Close();

  check_.Stop();
  check_.Close();
}

void AudioSystem::OnCheck() {
  std::ranges::for_each(stopped_, AudioSourceBuffer::Destroy);
  stopped_.clear();
}

void AudioSystem::OnTick() {
  std::erase_if(playing_, [this](const AudioSourceBuffer& sfx) {
    if (!sfx.source.IsStopped())
      return false;
    DLOG(INFO) << sfx.source.GetSourceId() << " stopped!";
    stopped_.push_back(sfx);
    return true;
  });
}

void AudioSystem::Play(const AudioSource& source, const AudioBuffer& buffer) {
  DLOG(INFO) << "playing: " << source;
  playing_.emplace_back(source, buffer);
  source.Play();
}

void AudioSystem::Play(const AudioSource& source, const fs::path path) {
  ASSERT(fs::exists(path) && fs::is_regular_file(path));
  AudioBufferBuilder builder{};
  AudioBuffer buffer = builder.WithWavFile(path);
  Play(source, buffer);
}

void AudioSystem::Play(const fs::path path) {
  ASSERT(fs::exists(path) && fs::is_regular_file(path));
  AudioSource source(true);
  source.SetPitch(1.0f);
  source.SetGain(1.0f);
  source.SetPos(0.0f);
  source.SetLooping(false);
  return Play(source, path);
}
}  // namespace prt::audio