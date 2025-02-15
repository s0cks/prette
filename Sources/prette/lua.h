#ifndef PRT_LUA_H
#define PRT_LUA_H

#include <filesystem>
#include <string>

#include "prette/common.h"
#include "prette/event.h"

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
#define FOR_EACH_LUA_STATE_EVENT(V) V(LuaStateInit)

class LuaState;
class LuaStateEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_LUA_STATE_EVENT(FORWARD_DECLARE);
#undef FORWARD_DECLARE

class LuaStateEvent : public Event {
 protected:
  LuaStateEvent() = default;

 public:
  ~LuaStateEvent() override = default;
  DEFINE_EVENT_PROTOTYPE(LuaState, FOR_EACH_LUA_STATE_EVENT);
};

class LuaStateInitEvent : public LuaStateEvent {
 private:
  lua_State* state_;

 public:
  explicit LuaStateInitEvent(lua_State* state) :
    LuaStateEvent(),
    state_(state) {}
  ~LuaStateInitEvent() override = default;

  auto GetState() const -> lua_State* {
    return state_;
  }

  DECLARE_EVENT_TYPE(LuaStateEvent, LuaStateInit);
};

DEFINE_EVENT_SUBJECT(LuaState);
DEFINE_EVENT_OBSERVABLE(LuaState);
FOR_EACH_LUA_STATE_EVENT(DEFINE_EVENT_OBSERVABLE);

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
  LuaState(const std::filesystem::path& root);

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
