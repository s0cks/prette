#ifndef PRT_WORLD_EVENT_H
#define PRT_WORLD_EVENT_H

#include "prette/event.h"
#include "prette/world/world_state.h"

namespace prt {
// clang-format off
#define FOR_EACH_WORLD_EVENT(V) \
  FOR_EACH_WORLD_STATE(V)
// clang-format on

class World;
class WorldEvent;
#define FORWARD_DECLARE_WORLD_EVENT(Name) class Name##Event;
FOR_EACH_WORLD_EVENT(FORWARD_DECLARE_WORLD_EVENT)
#undef FORWARD_DECLARE_WORLD_EVENT

DEFINE_EVENT_PROTOTYPE(World, FOR_EACH_WORLD_EVENT);

#define DEFINE_WORLD_EVENT_TYPE(Name) DECLARE_EVENT_TYPE(WorldEvent, Name)

#define DEFINE_WORLD_EVENT(Name)          \
  class Name##Event : public WorldEvent { \
   public:                                \
    Name##Event() = default;              \
    ~Name##Event() override = default;    \
    DEFINE_WORLD_EVENT_TYPE(Name);        \
  };

FOR_EACH_WORLD_STATE(DEFINE_WORLD_EVENT);

DEFINE_EVENT_SUBJECT(World);
DEFINE_EVENT_OBSERVABLE(World);
FOR_EACH_WORLD_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_WORLD_EVENT_H
