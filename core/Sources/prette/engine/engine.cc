#include "prette/engine/engine.h"

#include <cctype>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <gflags/gflags.h>
#include <lua.h>
#include <marl/defer.h>
#include <marl/scheduler.h>
#include <memory>
#include <uv.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/config/config_system.h"
#include "prette/crash_report.h"
#include "prette/ecl.h"
#include "prette/engine/engine_event.h"
#include "prette/engine/engine_lua_bindings.h"
#include "prette/engine/engine_states.h"
#include "prette/event.h"
#include "prette/jsonnet.h"
#include "prette/lua.h"
#include "prette/lua_event.h"
#include "prette/os_thread.h"
#include "prette/rx.h"
#include "prette/settings/settings_system.h"
#include "prette/signals.h"
#include "prette/thread_local.h"
#include "prette/uv/utils.h"
#include "prette/world/world_manager.h"

namespace prt {
DEFINE_GLOBAL_EVENT_SUBJECT(EngineEvent, engine_events);
static ThreadLocal<Engine> engine_;

static inline auto SetEngine(Engine* engine) -> Engine* {
  ASSERT(engine);
  engine_.Set(engine);
  return engine;
}

void Engine::PublishEvent(EngineEvent* event) {
  ASSERT(event);
  const auto& subscriber = engine_events.get_subscriber();
  return subscriber.on_next(event);
}

auto IsEngineInitialized() -> bool {
  return engine_.Get() != nullptr;
}

auto GetEngine() -> Engine* {
  ASSERT(IsEngineInitialized());
  return engine_.Get();
}

void Engine::Terminate() {
  if (cause_)
    return SetState<ErrorState>(cause_);
  return SetState<TerminatingState>();
}

static inline auto CreateTickDeltaObservable(const Engine* engine) -> rx::observable<uint64_t> {
  return OnTickEvent().map([](TickEvent* event) {
    return (event->GetTimeSinceLast()).value();
  });
}

Engine::Engine() :
  loop_(),
  ticker_(loop_),
  scheduler_(&loop_),
  mscheduler_((marl::Scheduler::Config::allCores())),
#ifdef PRT_DEBUG
  tick_profiler_(CreateTickDeltaObservable(this)),
#endif  // PRT_DEBUG
  on_shutdown_(loop_, [this]() {
    return Terminate();
  }) {

  // marl scheduler
  mscheduler_.bind();

  // lua bindings
#ifdef PRT_ENABLE_LUA
  on_lua_init_ = OnLuaStateInit([this](LuaStateInitEvent* event) {
    EngineModule::Init(GetLua()->GetState());
  });
#endif  // PRT_ENABLE_LUA
}

Engine::~Engine() {
#ifdef PRT_ENABLE_LUA
  on_lua_init_.unsubscribe();
#endif  // PRT_ENABLE_LUA
  mscheduler_.unbind();
}

auto Engine::Run() -> int {
  SetState<InitState>();
  if (HasState()) {
    ExitState();
    ClearState();
  }
  return EXIT_SUCCESS;
}

void Engine::Shutdown(std::shared_ptr<CrashReportCause> cause) {
  cause_ = cause;
  on_shutdown_.Send();
}

auto Engine::Schedule(uv::Work* work) -> uv::Work* {
  return scheduler_.Schedule(work);
}

auto Engine::Schedule(uv::AnyWork::OnWorkCallback on_work) -> uv::Work* {
  return scheduler_.Schedule(on_work);
}

auto Engine::Schedule(uv::AnyWork::OnWorkCallback on_work, uv::AnyWork::OnWorkFinishedCallback on_finished)
    -> uv::Work* {
  return scheduler_.Schedule(on_work, on_finished);
}

static inline void OnUnhandledException() {
  CrashReport report(CrashReportCause::New(std::current_exception()));
  report.Print();
  LOG(FATAL) << "unhandled exception occured.";
}

void Engine::Init(int argc, char** argv) {
  // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
  ::google::InitGoogleLogging(argv[0]);
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  srand(time(nullptr));
  InitSignalHandlers();
  std::set_terminate(OnUnhandledException);
  LOG_IF(FATAL, !SetCurrentThreadName("main")) << "failed to set main thread name.";

  ASSERT(!IsEngineInitialized());
  SetEngine(new Engine());
  ASSERT(IsEngineInitialized());

  ConfigSystem::Init();
  SettingsSystem::InitSystem();
  InitLua();
  InitLisp(argc, argv);
  InitJsonnet();
  WorldManager::Init();
}
}  // namespace prt