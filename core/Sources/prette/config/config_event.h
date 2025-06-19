#ifndef PRT_CONFIG_EVENT_H
#define PRT_CONFIG_EVENT_H

#include "prette/config/config_state.h"
#include "prette/event.h"

namespace prt {
#define FOR_EACH_CONFIG_EVENT(V) FOR_EACH_CONFIG_STATE(V)

#define FORWARD_DECLARE(Name)    class Name##Event;
FOR_EACH_CONFIG_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Config, FOR_EACH_CONFIG_EVENT);

#define DECLARE_CONFIG_EVENT(Name)         \
  class Name##Event : public ConfigEvent { \
   public:                                 \
    Name##Event() = default;               \
    ~Name##Event() override = default;     \
    DECLARE_EVENT_TYPE(ConfigEvent, Name); \
  };
FOR_EACH_CONFIG_STATE(DECLARE_CONFIG_EVENT);
#undef DECLARE_CONFIG_EVENT

DEFINE_EVENT_SUBJECT(Config);
DEFINE_EVENT_OBSERVABLE(Config);
FOR_EACH_CONFIG_EVENT(DEFINE_EVENT_SUBJECT);
FOR_EACH_CONFIG_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_CONFIG_EVENT_H
