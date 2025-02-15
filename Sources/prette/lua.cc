#include "prette/lua.h"

#include <lauxlib.h>
#include <lua.h>

#include <array>
#include <filesystem>
#include <rx-coordination.hpp>
#include <subjects/rx-replaysubject.hpp>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/flags.h"
#include "prette/keyboard.h"
#include "prette/mouse.h"
#include "prette/prette.h"
#include "prette/renderer.h"
#include "prette/rx.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
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

auto LuaStateInitEvent::ToString() const -> std::string {
  return ToStringHelper<LuaStateInitEvent>{};
}

LUA_F(getVersion) {
  const auto version = prt::GetVersion();
  lua_pushstring(L, version.c_str());
  return 1;
}

LUA_F(log) {
  luaL_checktype(L, 1, LUA_TNUMBER);
  const auto level = lua_tointeger(L, 1);
  const auto value = lua_tostring(L, 2);
  LOG_AT_LEVEL(static_cast<google::LogSeverity>(level)) << value;
  return 0;
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
{.name="isDebug", .func=&lua_isDebug},
{.name="log", .func=&lua_log },
};
// clang-format on

LUA_F(prette_tostring) {
  ASSERT(L);
  const auto message = fmt::format("Prette v{}", prt::GetVersion());
  lua_pushstring(L, message.c_str());
  return 1;
}

void LuaState::InitLua(lua_State* L) {
  ASSERT(L);
  DLOG(INFO) << "initializing lua bindings....";
  luaL_openlibs(L);

  lua_newtable(L);

  luaL_newmetatable(L, "Prette");
  lua_pushcfunction(L, &lua_prette_tostring);
  lua_setfield(L, -2, "__tostring");
  lua_setmetatable(L, -2);

  luaL_setfuncs(L, kPretteLib, 0);
  lua_setglobal(L, "Prette");
  lua_pushinteger(L, google::INFO);
  lua_setglobal(L, "INFO");
  lua_pushinteger(L, google::ERROR);
  lua_setglobal(L, "ERROR");
  lua_pushinteger(L, google::WARNING);
  lua_setglobal(L, "WARNING");

  Engine::InitLua(L);
  Window::InitLua(L);
  Driver::InitLua(L);
  Keyboard::InitLua(L);
  Mouse::InitLua(L);
  Renderer::InitLua(L);
  Publish<LuaStateInitEvent>(L);
}

#define L state_

LuaState::LuaState(const std::filesystem::path& root) :
  state_(luaL_newstate()),
  root_(root) {
  ASSERT(std::filesystem::is_directory(root_));
}

LuaState::~LuaState() {
  lua_close(L);
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

static LuaState* state = nullptr;

auto LuaState::New(const std::filesystem::path& root) -> LuaState* {
  const auto state = new LuaState(root);
  ASSERT(state);
  return state;
}

void LuaState::Init() {
  ASSERT(state == nullptr);
  state = LuaState::New(std::filesystem::path(FLAGS_resources) / "scripts");
  ASSERT(state);
  InitLua(state->GetState());
}

auto LuaState::Get() -> LuaState* {
  ASSERT(state);
  return state;
}
}  // namespace prt