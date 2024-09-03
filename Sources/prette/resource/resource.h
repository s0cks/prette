#ifndef PRT_RESOURCE_H
#define PRT_RESOURCE_H

#include "prette/rx.h"
#include "prette/resource/resource_ptr.h"
#include "prette/resource/resource_type.h"
#include "prette/resource/resource_event.h"
#include "prette/resource/resource_loader.h"

namespace prt {
  namespace shader {
    class Shader;
  }

  namespace resource {
    class Resource {
    protected:
      Resource() = default;
      virtual void Destroy() = 0;
      virtual Type GetResourceType() const = 0;
    private:
      void Publish(ResourceEvent* event);

      template<class E, typename... Args>
      inline void Publish(Args... args) {
        E event(args...);
        return Publish(&event);
      }
    public:
      virtual ~Resource() = default;
    };

    template<const Type T>
    class ResourceTemplate : public Resource {
    protected:
      ResourceTemplate() = default;
    public:
      ~ResourceTemplate() override = default;
      
      Type GetResourceType() const override {
        return T;
      }
    };

    rx::observable<ResourceEvent*> OnResourceEvent();

#define DEFINE_ON_EVENT(Name)                                 \
    static inline rx::observable<Name##Event*>                \
    On##Name##Event() {                                       \
      return OnResourceEvent()                                \
        .filter(Name##Event::FilterBy)                        \
        .map(Name##Event::Cast);                              \
    }
    DEFINE_ON_EVENT(ResourceLoading);
    DEFINE_ON_EVENT(ResourceCreated);
    DEFINE_ON_EVENT(ResourceDestroyed);
#undef DEFINE_ON_EVENT
  }

  namespace res=resource;
}

#endif //PRT_RESOURCE_H