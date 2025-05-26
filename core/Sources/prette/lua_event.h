#ifndef PRT_LUA_EVENT_H
#define PRT_LUA_EVENT_H

#ifdef PRT_ENABLE_LUA

#include "prette/event.h"

namespace prt {
#define FOR_EACH_LUA_STATE_EVENT(V) \
  V(LuaStateInit)                   \
  V(LuaStateDestroyed)

class LuaState;
class LuaStateEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_LUA_STATE_EVENT(FORWARD_DECLARE);
#undef FORWARD_DECLARE

#define DEFINE_LUA_STATE_EVENT_TYPE(Name) DECLARE_EVENT_TYPE(LuaStateEvent, LuaState##Name)

DEFINE_EVENT_PROTOTYPE(LuaState, FOR_EACH_LUA_STATE_EVENT);

#define DEFINE_LUA_STATE_EVENT(Name)                   \
  class LuaState##Name##Event : public LuaStateEvent { \
   public:                                             \
    LuaState##Name##Event() = default;                 \
    ~LuaState##Name##Event() override = default;       \
    DEFINE_LUA_STATE_EVENT_TYPE(Name);                 \
  };

DEFINE_LUA_STATE_EVENT(Init);
DEFINE_LUA_STATE_EVENT(Destroyed);

DEFINE_EVENT_SUBJECT(LuaState);
DEFINE_EVENT_OBSERVABLE(LuaState);
FOR_EACH_LUA_STATE_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_ENABLE_LUA

#endif  // PRT_LUA_EVENT_H
