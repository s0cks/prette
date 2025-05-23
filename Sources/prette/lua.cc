#include "prette/lua.h"
#ifdef PRETTE_ENABLE_LUA

#include <filesystem>
#include <fmt/format.h>
#include <gflags/gflags.h>
#include <string>
#include <utility>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/flags.h"
#include "prette/keyboard.h"
#include "prette/lua_event.h"
#include "prette/mouse.h"
#include "prette/prette.h"
#include "prette/rx.h"
#include "prette/settings.h"
#include "prette/thread_local.h"
#include "prette/to_string.h"
#include "prette/window.h"
#include "prette/world.h"

namespace prt {
DEFINE_bool(lua_event_logging, false, "Enable logging of events sent to lua callbacks.");

static rx::subject<LuaStateEvent*> events_{};

static void PublishEvent(LuaStateEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

auto OnLuaStateEvent() -> LuaStateEventObservable {
  return events_.get_observable();
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

void LuaState::InitLua(lua_State* L) {
  ASSERT(L);
  luaL_openlibs(L);

  lua_pushcfunction(L, &lua_prette_print);
  lua_setglobal(L, "print");

  RuntimeModule::Init(L);
  LogModule::Init(L);
  Engine::InitLua(L);
  Driver::InitLua(L);
  Keyboard::InitLua(L);
  Mouse::InitLua(L);
  World::InitLua(L);
  Window::InitLua(L);
  Settings::InitLua(L);
}

#define L state_

LuaState::LuaState(fs::path root) :
  state_(luaL_newstate()),
  root_(std::move(root)) {
  ASSERT(std::filesystem::is_directory(root_));
  InitLua(L);
}

LuaState::~LuaState() {
  lua_close(L);
  Publish<LuaStateDestroyedEvent>();
}

void LuaState::ExecuteCode(const std::string& code) {
  ASSERT(!code.empty());
  if (luaL_dostring(L, code.c_str()) != LUA_OK)
    LOG(ERROR) << "error executing lua code: " << lua_tostring(L, lua_gettop(L));
  lua_pop(L, lua_gettop(L));
}

void LuaState::ExecuteScript(const std::string& path) {
  ASSERT(!path.empty());
  const auto script_path = GetRoot() / path;
  if (!fs::is_regular_file(script_path)) {
    DLOG(WARNING) << "cannot execute lua script " << path << ", script not found.";
    return;
  }
  const auto result = luaL_dofile(L, script_path.c_str());
  LOG_IF(ERROR, result != LUA_OK) << "error executing lua code: " << lua_tostring(L, lua_gettop(L));
  lua_pop(L, lua_gettop(L));
}

void LuaState::ExecuteGlobalFunction(const std::string& global, const std::string& func) {
  ASSERT(!global.empty());
  ASSERT(!func.empty());
  lua_getglobal(L, global.c_str());
  lua_getfield(L, -1, func.c_str());
  if (lua_type(L, -1) == LUA_TFUNCTION) {
    lua_call(L, 0, 0);
  }
  lua_pop(L, lua_gettop(L));
}

#undef L

static ThreadLocal<LuaState> thread_state_{};

auto LuaState::IsInitialized() -> bool {
  return thread_state_.Get() != nullptr;
}

auto LuaState::New(const std::filesystem::path& root) -> LuaState* {
  return new LuaState(root);
}

void LuaState::Init() {
  ASSERT(!IsInitialized());
  thread_state_ = LuaState::New(std::filesystem::path(FLAGS_resources) / "scripts");
  ASSERT(IsInitialized());
  Publish<LuaStateInitEvent>();
}

auto LuaState::Get() -> LuaState* {
  ASSERT(IsInitialized());
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

#endif  // PRETTE_ENABLE_LUA