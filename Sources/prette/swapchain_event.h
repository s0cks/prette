#ifndef PRT_SWAPCHAIN_EVENT_H
#define PRT_SWAPCHAIN_EVENT_H

#include "prette/event.h"

namespace prt {
#define FOR_EACH_SWAPCHAIN_EVENT(V) \
  V(SwapchainCreated)               \
  V(SwapchainInit)                  \
  V(SwapchainDestroyed)

class SwapchainEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_SWAPCHAIN_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Swapchain, FOR_EACH_SWAPCHAIN_EVENT);

class SwapchainCreatedEvent : public SwapchainEvent {
 public:
  SwapchainCreatedEvent() = default;
  ~SwapchainCreatedEvent() override = default;
  DECLARE_EVENT_TYPE(SwapchainEvent, SwapchainCreated);
};

class SwapchainInitEvent : public SwapchainEvent {
 private:
  bool reinit_;

 public:
  explicit SwapchainInitEvent(const bool reinit) :
    SwapchainEvent(),
    reinit_(reinit) {}
  ~SwapchainInitEvent() override = default;

  auto IsReinit() const -> bool {
    return reinit_;
  }

  DECLARE_EVENT_TYPE(SwapchainEvent, SwapchainInit);
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

DEFINE_EVENT_SUBJECT(Swapchain);
DEFINE_EVENT_OBSERVABLE(Swapchain);
FOR_EACH_SWAPCHAIN_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_SWAPCHAIN_EVENT_H
