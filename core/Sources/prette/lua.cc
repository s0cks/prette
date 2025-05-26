#include "prette/assertions.h"
#include "prette/event.h"
#ifdef PRT_ENABLE_LUA

#include <filesystem>
#include <fmt/format.h>
#include <gflags/gflags.h>
#include <string>
#include <utility>

#include "prette/common.h"
#include "prette/flags.h"
#include "prette/lua.h"
#include "prette/lua_event.h"
#include "prette/prette.h"
#include "prette/rx.h"
#include "prette/script_engine.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"

namespace prt {
DEFINE_bool(lua_event_logging, false, "Enable logging of events sent to lua callbacks.");

DEFINE_GLOBAL_EVENT_SUBJECT(LuaStateEvent, events);

static void PublishEvent(LuaStateEvent* event) {
  ASSERT(event);
  const auto& subscriber = events.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

class RuntimeModule : LuaModule {
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

LUA_F(getVersion) {
  const auto version = prt::GetVersion();
  lua_pushstring(L, version.c_str());
  return 1;
}

LUA_F(isDebug) {
#ifdef PRT_DEBUG
  lua_pushboolean(L, true);
#else
  lua_pushboolean(L, false);
#endif  // PRT_DEBUG
  return 1;
}

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kPretteLib[] = {
{.name="getVersion",.func=&lua_getVersion },
{.name="isDebug", .func=&lua_isDebug}
};
// clang-format on

LUA_F(prette_tostring) {
  ASSERT(L);
  const auto message = fmt::format("Prette v{}", prt::GetVersion());
  lua_pushstring(L, message.c_str());
  return 1;
}

void RuntimeModule::Init(lua_State* L) {
  InitModule(L, "runtime", "Runtime", kPretteLib, &lua_prette_tostring);

  lua_getglobal(L, "runtime");
  luaL_checktype(L, -1, LUA_TTABLE);
  lua_pushboolean(L, FLAGS_lua_event_logging);
  lua_setfield(L, -2, "logEvents");
}

class LogModule : public LuaModule {
  friend class LuaState;

 private:
  static void Init(lua_State* L);
};

LUA_F(log_log) {
  luaL_checktype(L, 1, LUA_TNUMBER);
  const auto level = lua_tointeger(L, 1);
  const auto value = lua_tostring(L, 2);
  LOG_AT_LEVEL(static_cast<google::LogSeverity>(level)) << value;
  return 0;
}

LUA_F(log_info) {
  const auto value = lua_tostring(L, 1);
  LOG(INFO) << value;
  return 0;
}

LUA_F(log_debug) {
  const auto value = lua_tostring(L, 1);
  DLOG(INFO) << value;
  return 0;
}

LUA_F(log_warning) {
  const auto value = lua_tostring(L, 1);
  LOG(WARNING) << value;
  return 0;
}

LUA_F(log_error) {
  const auto value = lua_tostring(L, 1);
  LOG(ERROR) << value;
  return 0;
}

LUA_F(log_isVerboseLevel) {
  const auto level = lua_tointeger(L, 1);
  if (VLOG_IS_ON(level)) {
    lua_pushboolean(L, true);
  } else {
    lua_pushboolean(L, false);
  }
  return 1;
}

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kLogModule[] = {
{.name="log",.func=&lua_log_log},
{.name = "info", .func = &lua_log_info },
{.name = "debug", .func = &lua_log_debug },
{.name = "warning", .func = &lua_log_warning },
{.name = "error", .func = &lua_log_error },
{.name = "isVerboseLevel", .func = &lua_log_isVerboseLevel },
};
// clang-format on

void LogModule::Init(lua_State* L) {
  InitModule(L, "log", "Log", kLogModule);
}

LUA_F(prette_print) {
  ASSERT(L);
  if (lua_isstring(L, 1)) {
    LOG(INFO) << lua_tostring(L, 1);
  } else if (lua_isnumber(L, 1)) {
    LOG(INFO) << lua_tonumber(L, 1);
  } else if (lua_isboolean(L, 1)) {
    LOG(INFO) << lua_toboolean(L, 1);
  } else if (lua_isnoneornil(L, 1)) {
    LOG(INFO) << "nil";
  } else if (lua_istable(L, 1)) {
    if (!luaL_callmeta(L, 1, "__tostring")) {
      LOG(WARNING) << "unsupported lua value.";
      return luaL_error(L, "unsupported lua value.");  // NOLINT(cppcoreguidelines-pro-type-vararg)
    }
    LOG(INFO) << luaL_checkstring(L, -1);
  }
  return 0;
}

#define L state_

LuaState::LuaState(fs::path root) :
  root_(std::move(root)) {
  ASSERT(std::filesystem::is_directory(root_));
}

LuaState::~LuaState() {
  on_preinit_.unsubscribe();
  lua_close(L);
  Publish<LuaStateDestroyedEvent>();
}

void LuaState::Init() {
  ScriptEngine::Init();
  InitLua();
}

void LuaState::Finalize() {
  ScriptEngine::Finalize();
  lua_close(state_);
}

void LuaState::InitLua() {
  state_ = luaL_newstate();
  luaL_openlibs(L);
  lua_pushcfunction(L, &lua_prette_print);
  lua_setglobal(L, "print");
  RuntimeModule::Init(L);
  LogModule::Init(L);
  Publish<LuaStateInitEvent>();
}

auto LuaState::ExecuteCode(const std::string code) -> bool {
  ASSERT_INITIALIZED(this);
  ASSERT(!code.empty());
  if (luaL_dostring(L, code.c_str()) != LUA_OK) {
    LOG(ERROR) << "error executing lua code: " << lua_tostring(L, lua_gettop(L));
    return false;
  }
  lua_pop(L, lua_gettop(L));
  return true;
}

auto LuaState::ExecuteScript(const std::string path) -> bool {
  ASSERT_INITIALIZED(this);
  ASSERT(!path.empty());
  const auto script_path = GetRoot() / path;
  if (!fs::is_regular_file(script_path)) {
    DLOG(WARNING) << "cannot execute lua script " << path << ", script not found.";
    return false;
  }
  const auto result = luaL_dofile(L, script_path.c_str());
  LOG_IF(ERROR, result != LUA_OK) << "error executing lua code: " << lua_tostring(L, lua_gettop(L));
  lua_pop(L, lua_gettop(L));
  return result == LUA_OK;
}

auto LuaState::ExecuteGlobalFunc(const std::string global) -> bool {
  ASSERT(!global.empty());
  lua_getglobal(L, global.c_str());
  luaL_checktype(L, -1, LUA_TFUNCTION);
  lua_call(L, 0, 0);
  lua_pop(L, lua_gettop(L));
  return true;
}

#undef L

static ThreadLocal<LuaState> thread_state_{};

auto IsLuaInitialized() -> bool {
  return thread_state_.Get() != nullptr;
}

void InitLua() {
  ASSERT(!IsLuaInitialized());
  thread_state_ = new LuaState(std::filesystem::path(FLAGS_resources) / "scripts");
  ASSERT(IsLuaInitialized());
}

auto GetLua() -> LuaState* {
  return thread_state_;
}

LUA_F(module_default_tostring) {
  lua_pushstring(L, "PretteModule");
  return 1;
}

void LuaModule::InitModule(lua_State* L, const char* name, const char* tname, const struct luaL_Reg* lib_funcs,
                           lua_CFunction tostring, const int nup) {
  DVLOG(1) << "creating " << tname << " lua module as global " << name;
  lua_newtable(L);

  luaL_newmetatable(L, tname);
  lua_pushcfunction(L, tostring);
  lua_setfield(L, -2, "__tostring");
  lua_setmetatable(L, -2);

  luaL_setfuncs(L, lib_funcs, nup);
  lua_setglobal(L, name);
}

void LuaModule::InitModule(lua_State* L, const char* name, const char* tname, const struct luaL_Reg* lib_funcs,
                           const int nup) {
  return InitModule(L, name, tname, lib_funcs, &lua_module_default_tostring, nup);
}
}  // namespace prt

#endif  // PRT_ENABLE_LUA