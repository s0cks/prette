#ifndef PRT_SWAPCHAIN_EVENT_H
#define PRT_SWAPCHAIN_EVENT_H

#include "prette/event.h"
#include "prette/swapchain/swapchain_state.h"

namespace prt {
#define FOR_EACH_SWAPCHAIN_EVENT(V) FOR_EACH_SWAPCHAIN_STATE(V)

class SwapchainEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_SWAPCHAIN_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Swapchain, FOR_EACH_SWAPCHAIN_EVENT);

#define DECLARE_SWAPCHAIN_EVENT(Name)         \
  class Name##Event : public SwapchainEvent { \
   public:                                    \
    Name##Event() = default;                  \
    ~Name##Event() override = default;        \
    DECLARE_EVENT_TYPE(SwapchainEvent, Name); \
  };

DECLARE_SWAPCHAIN_EVENT(SwapchainInit);
class SwapchainCreatedEvent : public SwapchainEvent {
 private:
  bool reinit_;

 public:
  explicit SwapchainCreatedEvent(const bool reinit) :
    SwapchainEvent(),
    reinit_(reinit) {}
  ~SwapchainCreatedEvent() override = default;

  auto IsReinit() const -> bool {
    return reinit_;
  }

  DECLARE_EVENT_TYPE(SwapchainEvent, SwapchainCreated);
};

class SwapchainDestroyedEvent : public SwapchainEvent {
 private:
  bool reinit_;

 public:
  explicit SwapchainDestroyedEvent(const bool reinit) :
    SwapchainEvent(),
    reinit_(reinit) {}
  ~SwapchainDestroyedEvent() override = default;

  auto IsReinit() const -> bool {
    return reinit_;
  }

  DECLARE_EVENT_TYPE(SwapchainEvent, SwapchainDestroyed);
};

DECLARE_SWAPCHAIN_EVENT(SwapchainRecreated);
DECLARE_SWAPCHAIN_EVENT(SwapchainDeInit);

DEFINE_EVENT_SUBJECT(Swapchain);
DEFINE_EVENT_OBSERVABLE(Swapchain);
FOR_EACH_SWAPCHAIN_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_SWAPCHAIN_EVENT_H
