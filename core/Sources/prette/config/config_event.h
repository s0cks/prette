#ifndef PRT_CONFIG_EVENT_H
#define PRT_CONFIG_EVENT_H

#include <string>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/config/config_state.h"
#include "prette/event.h"
#include "prette/rx.h"

namespace prt {
#define FORWARD_DECLARE_CONFIG_EVENT(Name) class Config##Name##Event;
FOR_EACH_CONFIG_STATE(FORWARD_DECLARE_CONFIG_EVENT)
#undef FORWARD_DECLARE_CONFIG_EVENT

class ConfigEvent : public Event {
 protected:
  ConfigEvent() = default;

 public:
  ~ConfigEvent() override = default;

#define DEFINE_TYPE_CHECK(Name)                                \
  virtual auto AsConfig##Name##Event()->Config##Name##Event* { \
    return nullptr;                                            \
  }                                                            \
  auto IsConfig##Name##Event()->bool {                         \
    return AsConfig##Name##Event() != nullptr;                 \
  }
  FOR_EACH_CONFIG_STATE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
};

#define DECLARE_CONFIG_STATE_EVENT(Name)                                  \
  class Config##Name##Event : public ConfigEvent {                        \
    DEFINE_NON_COPYABLE_TYPE(Config##Name##Event);                        \
                                                                          \
   public:                                                                \
    using ParentEventType = ConfigEvent;                                  \
                                                                          \
   public:                                                                \
    Config##Name##Event() = default;                                      \
    ~Config##Name##Event() override = default;                            \
    auto ToString() const -> std::string override;                        \
    auto GetName() const -> const char* override {                        \
      return "Config" #Name;                                              \
    }                                                                     \
    auto AsConfig##Name##Event() -> Config##Name##Event* override {       \
      return this;                                                        \
    }                                                                     \
    static inline auto Filter(ConfigEvent* event) -> bool {               \
      return event && event->IsConfig##Name##Event();                     \
    }                                                                     \
    static inline auto Cast(ConfigEvent* event) -> Config##Name##Event* { \
      ASSERT(event);                                                      \
      ASSERT(event->IsConfig##Name##Event());                             \
      return event->AsConfig##Name##Event();                              \
    }                                                                     \
  };

FOR_EACH_CONFIG_STATE(DECLARE_CONFIG_STATE_EVENT);

DEFINE_EVENT_SUBJECT(Config);
DEFINE_EVENT_OBSERVABLE(Config);

#define DECLARE_CONFIG_EVENT_OBSERVABLE(Name) \
  using Config##Name##EventObservable = rx::observable<Config##Name##Event*>;
FOR_EACH_CONFIG_STATE(DECLARE_CONFIG_EVENT_OBSERVABLE);
#undef DECLARE_CONFIG_EVENT_OBSERVABLE
}  // namespace prt

#endif  // PRT_CONFIG_EVENT_H
