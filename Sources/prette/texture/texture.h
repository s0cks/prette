#ifndef PRT_TEXTURE_H
#define PRT_TEXTURE_H

#include <set>
#include <optional>
#include "prette/gfx.h"
#include "prette/uri.h"
#include "prette/json.h"
#include "prette/platform.h"
#include "prette/resource/resource.h"

#include "prette/gfx_object.h"

#include "prette/texture/texture_id.h"
#include "prette/texture/texture_json.h"
#include "prette/texture/texture_slot.h"
#include "prette/texture/texture_events.h"
#include "prette/texture/texture_constants.h"


namespace prt {
  namespace texture {
    class Texture;
    class TextureVisitor {
    protected:
      TextureVisitor() = default;
    public:
      virtual ~TextureVisitor() = default;
      virtual bool VisitTexture(Texture* texture) = 0;
    };
    
    rx::observable<TextureEvent*> OnTextureEvent();

    static inline rx::observable<TextureEvent*>
    OnTextureEvent(const TextureId id) {
      return OnTextureEvent()
        .filter([id](TextureEvent* event) {
          return event
              && event->GetTextureId() == id;
        });
    }

#define DEFINE_ON_TEXTURE_EVENT(Name)                       \
    static inline rx::observable<Name##Event*>              \
    On##Name##Event() {                                     \
      return OnTextureEvent()                               \
        .filter(Name##Event::Filter)                        \
        .map(Name##Event::Cast);                            \
    }                                                       \
    static inline rx::observable<Name##Event*>              \
    On##Name##Event(const TextureId id) {                   \
      return OnTextureEvent()                               \
        .filter(Name##Event::FilterBy(id))                  \
        .map(Name##Event::Cast);                            \
    }
  //TODO:
#undef DEFINE_ON_TEXTURE_EVENT

    class Texture : public gfx::Object<TextureId> {
      template<const TextureSlot Slot>
      friend class TextureBindScope;

      friend class TextureBuilder;
      friend class TextureBuilderBase;
    public:
      static rx::observable<TextureId> GenerateTextureId(const int num = 1);

      struct Comparator {
        bool operator()(Texture* lhs, Texture* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    protected:
      TextureId id_;

      static void Publish(TextureEvent* event);

      static void BindTexture(const TextureTarget target, const TextureId id);
      static void DeleteTextures(const TextureId* ids, const uint64_t num_ids);
      static void ActiveTexture(const int32_t slot);

      static inline void
      DeleteTexture(const TextureId id) {
        return DeleteTextures(&id, 1);
      }
      
      static inline void
      UnbindTexture(const TextureTarget target) {
        return BindTexture(target, kInvalidTextureId);
      }

      template<class E, typename... Args>
      void Publish(Args... args) const {
        E event(GetId(), args...);
        return Publish(&event);
      }
    public:
      explicit Texture(const TextureId id);
      ~Texture() override;
      virtual TextureTarget GetTextureTarget() const = 0;

      bool Accept(TextureVisitor* vis);
      TextureWrap GetTextureWrap() const;
    };
    
    template<const TextureTarget Target>
    class TextureTemplate : public Texture {
    protected:
      explicit TextureTemplate(const TextureId id):
        Texture(id) {
      }
    public:
      ~TextureTemplate() override = default;

      TextureTarget GetTextureTarget() const override {
        return Target;
      }
    };

    typedef std::set<Texture*, Texture::Comparator> TextureSet;

    const TextureSet& GetAllTextures();
    uword GetTotalNumberOfTextures();
    bool VisitAllTextures(TextureVisitor* vis);
  }
  using texture::Texture;
}

#include "prette/texture/texture_2d/texture_2d.h"
#include "prette/texture/texture_3d/texture_3d.h"
#include "prette/cube_map/cube_map.h"

#endif //PRT_TEXTURE_H