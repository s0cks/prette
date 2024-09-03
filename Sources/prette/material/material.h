#ifndef PRT_MATERIAL_H
#define PRT_MATERIAL_H

#include <fstream>
#include <memory>

#include "prette/gfx.h"
#include "prette/flags.h"
#include "prette/buffer.h"

#include "prette/json.h"
#include "prette/texture/texture.h"
#include "prette/pipeline.h"

#include "prette/material/material_events.h"
#include "prette/material/material_registry.h"
#include "prette/material/material_component.h"

namespace prt {
  namespace material {
    const MaterialRegistry& GetRegistry();
    rx::observable<MaterialEvent*> OnMaterialEvent();

    static inline rx::observable<MaterialEvent*>
    OnMaterialEvent(const std::string& name) {
      return OnMaterialEvent()
        .filter([name](MaterialEvent* event) {
          return event
              && EqualsIgnoreCase(event->GetMaterialName(), name);
        });
    }

#define DEFINE_ON_MATERIAL_EVENT(Name)                        \
    static inline rx::observable<Name##Event*>                \
    On##Name##Event() {                                       \
      return OnMaterialEvent()                                \
        .filter(Name##Event::Filter)                          \
        .map(Name##Event::Cast);                              \
    }                                                         \
    static inline rx::observable<Name##Event*>                \
    On##Name##Event(const std::string& name) {                \
      return OnMaterialEvent()                                \
        .filter(Name##Event::FilterByName(name))              \
        .map(Name##Event::Cast);                              \
    }
    FOR_EACH_MATERIAL_EVENT(DEFINE_ON_MATERIAL_EVENT)
#undef DEFINE_ON_MATERIAL_EVENT

    class Material {
      friend class MaterialBuilder;
    protected:
      std::string name_;
      MaterialComponentSet components_;

      Material(const std::string& name,
               const MaterialComponentSet& components);

      static void Publish(MaterialEvent* event);

      template<class E, typename... Args>
      static inline void
      Publish(Args... args) {
        E event(args...);
        return Publish((MaterialEvent*) &event);
      }
    public:
      virtual ~Material();

      const std::string& GetName() const {
        return name_;
      }

      const MaterialComponentSet& GetComponents() const {
        return components_;
      }

      inline rx::observable<MaterialEvent*> OnEvent() const {
        return OnMaterialEvent(GetName());
      }
#define DEFINE_ON_MATERIAL_EVENT(Name)                                  \
      inline rx::observable<Name##Event*> On##Name##Event() const {     \
        return material::On##Name##Event(GetName());                    \
      }
      FOR_EACH_MATERIAL_EVENT(DEFINE_ON_MATERIAL_EVENT)
#undef DEFINE_ON_MATERIAL_EVENT

      virtual std::string ToString() const;
    public:
      static inline Material* 
      New(const std::string& name, const MaterialComponentSet& components) {
        return new Material(name, components);
      }
      
      static Material* New(const uri::Uri& uri);

      static inline Material*
      New(const uri::basic_uri& uri) {
        return New(uri::Uri(uri));
      }
    };
  }

  using material::Material;

  namespace resource {
    typedef Reference<Material> MaterialRef;
  }

  using resource::MaterialRef;

  MaterialRef GetMaterial(const uri::Uri& uri);

  static inline MaterialRef
  GetMaterial(const uri::basic_uri& uri) {
    return GetMaterial(uri::Uri(uri));
  }
}

#endif //PRT_MATERIAL_H