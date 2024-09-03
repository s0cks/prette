#ifndef PRT_MATERIAL_EVENTS_H
#define PRT_MATERIAL_EVENTS_H

#include "prette/event.h"
#include "prette/common.h"

namespace prt::material {
#define FOR_EACH_MATERIAL_EVENT(V) \
  V(MaterialCreated)               \
  V(MaterialDestroyed)

  class Material;
  class MaterialEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_MATERIAL_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  typedef std::function<bool(MaterialEvent*)> MaterialEventFilter;

  class MaterialEvent : public Event {
  protected:
    const Material* material_;

    explicit MaterialEvent(const Material* material):
      Event(),
      material_(material) {
    }
  public:
    ~MaterialEvent() override = default;

    const Material* GetMaterial() const {
      return material_;
    }

    const std::string& GetMaterialName() const;

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_MATERIAL_EVENT);
  };

#define DECLARE_MATERIAL_EVENT(Name)                                    \
  DECLARE_EVENT_TYPE(MaterialEvent, Name)                               \
  static inline MaterialEventFilter                                     \
  FilterByName(const std::string& name) {                               \
    return [name](MaterialEvent* event) {                               \
      return event                                                      \
          && EqualsIgnoreCase(event->GetMaterialName(), name);          \
    };                                                                  \
  }

  class MaterialCreatedEvent : public MaterialEvent {
  public:
    explicit MaterialCreatedEvent(const Material* material):
      MaterialEvent(material) {
    }
    ~MaterialCreatedEvent() override = default;
    DECLARE_MATERIAL_EVENT(MaterialCreated);
  };

  class MaterialDestroyedEvent : public MaterialEvent {
  public:
    explicit MaterialDestroyedEvent(const Material* material):
      MaterialEvent(material) {
    }
    ~MaterialDestroyedEvent() override = default;
    DECLARE_MATERIAL_EVENT(MaterialDestroyed);
  };
}

#endif //PRT_MATERIAL_EVENTS_H