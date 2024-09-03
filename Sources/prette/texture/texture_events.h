#ifndef PRT_TEXTURE_EVENT_H
#define PRT_TEXTURE_EVENT_H

#include "prette/event.h"
#include "prette/texture/texture_id.h"

namespace prt::texture {
#define FOR_EACH_TEXTURE_EVENT(V) \
  V(TextureCreated)               \
  V(TextureDestroyed)

  class TextureEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_TEXTURE_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class TextureEvent : public Event {
  protected:
    TextureId id_;

    explicit TextureEvent(const TextureId id):
      Event(),
      id_(id) {  
    }
  public:
    ~TextureEvent() override = default;

    TextureId GetTextureId() const {
      return id_;
    }
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_TEXTURE_EVENT);
  };

#define DECLARE_TEXTURE_EVENT(Name)           \
  DECLARE_EVENT_TYPE(TextureEvent, Name)

  class TextureCreatedEvent : public TextureEvent {
  public:
    explicit TextureCreatedEvent(const TextureId id):
      TextureEvent(id) {
    }
    ~TextureCreatedEvent() override = default;
    DECLARE_TEXTURE_EVENT(TextureCreated);
  };

  class TextureDestroyedEvent : public TextureEvent {
  public:
    explicit TextureDestroyedEvent(const TextureId id):
      TextureEvent(id) {
    }
    ~TextureDestroyedEvent() override = default;
    DECLARE_TEXTURE_EVENT(TextureDestroyed);
  };
}

#endif //PRT_TEXTURE_EVENT_H