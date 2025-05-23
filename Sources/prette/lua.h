#ifndef PRT_LUA_H
#define PRT_LUA_H

#ifdef PRETTE_ENABLE_LUA

#include <filesystem>
#include <functional>
#include <gflags/gflags.h>
#include <string>

#include "prette/common.h"
#include "prette/lua_event.h"

// IWYU pragma: begin_exports
#ifdef __cplusplus
extern "C" {
#endif

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#ifdef __cplusplus
}
#endif
// IWYU pragma: end_exports

namespace prt {
DECLARE_bool(lua_event_logging);

static inline auto ShouldEnableLuaEventLogging() -> bool {
  return FLAGS_lua_event_logging;
}

auto OnLuaStateEvent() -> LuaStateEventObservable;
#define DEFINE_ON_EVENT(Name)                                                    \
  static inline auto On##Name##Event()->Name##EventObservable {                  \
    return OnLuaStateEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_LUA_STATE_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

class LuaState {
 private:
  lua_State* state_;
  std::filesystem::path root_;

 protected:
  explicit LuaState(fs::path root);

 public:
  ~LuaState();

  auto GetState() const -> lua_State* {
    return state_;
  }

  auto GetRoot() const -> const std::filesystem::path& {
    return root_;
  }

  auto GetScriptPath(const std::string& filename) const -> std::filesystem::path {
    return GetRoot() / filename;
  }

  void ExecuteCode(const std::string& code);
  void ExecuteScript(const std::string& path);
  void ExecuteGlobalFunction(const std::string& global, const std::string& func);

 private:
  static auto New(const std::filesystem::path& root) -> LuaState*;
  static void InitLua(lua_State* L);

 public:
  static void Init();
  static auto IsInitialized() -> bool;
  static auto Get() -> LuaState*;
};

#define EXTERN_LUA  extern "C" inline

#define LUA_F(Name) EXTERN_LUA auto lua_##Name(lua_State* L)->int

class LuaModule {
 protected:
  static void InitModule(lua_State* L, const char* name, const char* tname, const struct luaL_Reg* lib_funcs,
                         lua_CFunction tostring, const int nup = 0);
  static void InitModule(lua_State* L, const char* name, const char* tname, const struct luaL_Reg* lib_funcs,
                         const int nup = 0);
};

#ifndef LUA_SUCCESS
#define LUA_SUCCESS 1
#endif  // LUA_SUCCESS

template <class E>
static inline auto CreateSubscriber(lua_State* L, const int index = 1) -> std::function<void(E*)> {
  luaL_checktype(L, index, LUA_TFUNCTION);
  const auto callback = luaL_ref(L, LUA_REGISTRYINDEX);
  return [L, callback](E* event) {
    ASSERT(event);
    lua_rawgeti(L, LUA_REGISTRYINDEX, callback);
    luaL_checktype(L, 1, LUA_TFUNCTION);
    event->ToTable(L);
    lua_call(L, 1, 0);
  };
}
}  // namespace prt

#endif  // PRETTE_ENABLE_LUA
#endif  // PRT_LUA_H
