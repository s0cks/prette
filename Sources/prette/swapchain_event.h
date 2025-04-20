#ifndef PRT_SWAPCHAIN_EVENT_H
#define PRT_SWAPCHAIN_EVENT_H

#include "prette/event.h"

namespace prt {
#define FOR_EACH_SWAPCHAIN_EVENT(V) \
  V(SwapChainInit)                  \
  V(SwapChainDestroyed)

class SwapChainEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_SWAPCHAIN_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class SwapChainEvent : public Event {
 private:
  bool reinit_;

 public:
  explicit SwapChainEvent(const bool reinit) :
    Event(),
    reinit_(reinit) {}
  ~SwapChainEvent() override = default;

  auto IsReinit() const -> bool {
    return reinit_;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(SwapChain, FOR_EACH_SWAPCHAIN_EVENT);
};

class SwapChainInitEvent : public SwapChainEvent {
 public:
  explicit SwapChainInitEvent(const bool reinit) :
    SwapChainEvent(reinit) {}
  ~SwapChainInitEvent() override = default;
  DECLARE_EVENT_TYPE(SwapChainEvent, SwapChainInit);
};

class SwapChainDestroyedEvent : public SwapChainEvent {
 public:
  explicit SwapChainDestroyedEvent(const bool reinit) :
    SwapChainEvent(reinit) {}
  ~SwapChainDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(SwapChainEvent, SwapChainDestroyed);
};

DEFINE_EVENT_SUBJECT(SwapChain);
DEFINE_EVENT_OBSERVABLE(SwapChain);
FOR_EACH_SWAPCHAIN_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_SWAPCHAIN_EVENT_H
