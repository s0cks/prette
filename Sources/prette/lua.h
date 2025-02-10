#ifndef PRT_LUA_H
#define PRT_LUA_H

#include <filesystem>
#include <string>

#include "prette/common.h"
#include "prette/rx.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#ifdef __cplusplus
}
#endif

namespace prt {
class LuaState {
 private:
  lua_State* state_;
  std::filesystem::path root_;

 protected:
  LuaState(const std::filesystem::path& root);

 public:
  ~LuaState();

  auto GetRoot() const -> const std::filesystem::path& {
    return root_;
  }

  void ExecuteCode(const std::string& code);
  void ExecuteScript(const std::string& path);

 private:
  static auto New(const std::filesystem::path& root) -> LuaState*;
  static void InitLua(lua_State* L);

 public:
  static void Init();
  static auto Get() -> LuaState*;
};

#define EXTERN_LUA  extern "C" inline

#define LUA_F(Name) EXTERN_LUA auto lua_##Name(lua_State* L)->int

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

#endif  // PRT_LUA_H
