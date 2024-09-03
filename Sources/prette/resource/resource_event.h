#ifndef PRT_RESOURCE_EVENT_H
#define PRT_RESOURCE_EVENT_H

#include <string>
#include <ostream>

#include "prette/event.h"

namespace prt {

  namespace resource {
#define FOR_EACH_RESOURCE_EVENT(V) \
  V(ResourceLoading)               \
  V(ResourceCreated)                \
  V(ResourceDestroyed)

    class ResourceEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
    FOR_EACH_RESOURCE_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

    class ResourceEvent : public Event {
    protected:
      ResourceEvent() = default;
    public:
      ~ResourceEvent() override = default;
#define TYPE_CHECK(Name)                                                  \
      virtual Name##Event* As##Name##Event() { return nullptr; }          \
      bool Is##Name##Event() { return As##Name##Event() != nullptr; }
      FOR_EACH_RESOURCE_EVENT(TYPE_CHECK)
#undef TYPE_CHECK

      friend std::ostream& operator<<(std::ostream& stream, const ResourceEvent& rhs) {
        stream << rhs.ToString();
        return stream;
      }
    };

#define DECLARE_RESOURCE_EVENT(Name)                                        \
    public:                                                                 \
      const char* GetName() const override { return #Name; }                \
      std::string ToString() const override;                                \
      static inline bool FilterBy(ResourceEvent* event) {                   \
        return event && event->Is##Name##Event();                           \
      }                                                                     \
      static inline Name##Event*                                            \
      Cast(ResourceEvent* event) {                                          \
        return event ? nullptr : event->As##Name##Event();                  \
      }

    class ResourceLoadingEvent : public ResourceEvent {
      friend class Resource;
    protected:
      ResourceLoadingEvent() = default;
    public:
      ~ResourceLoadingEvent() override = default;
      DECLARE_RESOURCE_EVENT(ResourceLoading);

      friend std::ostream& operator<<(std::ostream& stream, const ResourceLoadingEvent& rhs) {
        stream << "ResourceLoadingEvent(";
        stream << ")";
        return stream;
      }
    };

    class ResourceCreatedEvent : public ResourceEvent {
      friend class Resource;
    protected:
      ResourceCreatedEvent() = default;
    public:
      ~ResourceCreatedEvent() override = default;
      DECLARE_RESOURCE_EVENT(ResourceCreated);

      friend std::ostream& operator<<(std::ostream& stream, const ResourceCreatedEvent& rhs) {
        stream << "ResourceCreatedEvent(";
        stream << ")";
        return stream;
      }
    };

    class ResourceDestroyedEvent : public ResourceEvent {
      friend class Resource;
    protected:
      ResourceDestroyedEvent() = default;
    public:
      ~ResourceDestroyedEvent() override = default;
      DECLARE_RESOURCE_EVENT(ResourceDestroyed);

      friend std::ostream& operator<<(std::ostream& stream, const ResourceDestroyedEvent& rhs) {
        stream << "ResourceDestroyedEvent(";
        stream << ")";
        return stream;
      }
    };
  }
}

#endif //PRT_RESOURCE_EVENT_H