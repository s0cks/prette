#include <backward.hpp>
#include <cstdlib>
#include <glog/logging.h>
#include <units.h>

#include "prette/assertions.h"
#include "prette/audio/audio_system.h"
#include "prette/common.h"
#include "prette/config/config.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/flags.h"
#include "prette/lua.h"
#include "prette/rx.h"
#include "prette/settings/settings.h"

using namespace prt;

auto main(int argc, char** argv) -> int {
  Engine::Init(argc, argv);
  audio::AudioSystem::InitSystem();
  if (VLOG_IS_ON(1)) {
    SUBSCRIBE_AND_LOG(GetConfigEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetSettingsEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetLuaStateEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetNonTickEngineEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(audio::GetAudioEventObservable(), INFO);
  }

  OnPostInit([](PostInitEvent* event) {
    audio::AudioSystem::GetSystem()->Play(fs::path(FLAGS_resources) / "sfx" / "button-click.wav");
  });

  const auto engine = GetEngine();
  ASSERT(engine);
  return engine->Run();
}