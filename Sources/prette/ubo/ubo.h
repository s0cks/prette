#ifndef PRT_UBO_H
#define PRT_UBO_H

#include "prette/ubo/ubo_id.h"
#include "prette/ubo/ubo_events.h"
#include "prette/gfx_buffer_object.h"

namespace prt {
  namespace ubo {
    class Ubo;
    class UboVisitor {
    protected:
      UboVisitor() = default;
    public:
      virtual ~UboVisitor() = default;
      virtual bool VisitUbo(Ubo* ubo) = 0;
    };

    rx::observable<UboEvent*> OnUboEvent();

    static inline rx::observable<UboEvent*>
    OnUboEvent(const UboId id) {
      return OnUboEvent()
        .filter([id](UboEvent* event) {
          return event
              && event->GetUboId() == id;
        });
    }

#define DEFINE_ON_UBO_EVENT(Name)                               \
    static inline rx::observable<Name##Event*>                  \
    On##Name##Event() {                                         \
      return OnUboEvent()                                       \
        .filter(Name##Event::Filter)                            \
        .map(Name##Event::Cast);                                \
    }                                                           \
    static inline rx::observable<Name##Event*>                  \
    On##Name##Event(const UboId id) {                           \
      return OnUboEvent()                                       \
        .filter(Name##Event::FilterBy(id))                      \
        .map(Name##Event::Cast);                                \
    }
    FOR_EACH_UBO_EVENT(DEFINE_ON_UBO_EVENT)
#undef DEFINE_ON_UBO_EVENT

    static constexpr const auto kGlTarget = GL_UNIFORM_BUFFER;

    class Ubo : public gfx::BufferObject<UboId, kGlTarget> {
      friend class UboBindScope;
      friend class UboUpdateScope;
      friend class UboBuilderBase;
      friend class UboBuilderScope;
    public:
      struct IdComparator {
        bool operator()(Ubo* lhs, Ubo* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    protected:
      explicit Ubo(const UboId id,
                   Class* cls,
                   const uword length,
                   const gfx::Usage usage);

      static void Publish(UboEvent* event);

      template<class E, typename... Args>
      static inline void
      Publish(Args... args) {
        E event(args...);
        return Publish((UboEvent*) &event);
      }

      static void BindUbo(const UboId id);
      static void DeleteUbos(const UboId* ids, const uint64_t num_ids);
      static void InitUbo(const uint8_t* bytes, const uint64_t num_bytes, const GLenum usage = GL_DYNAMIC_DRAW); //TODO: add usage
      static void UpdateUbo(const uint64_t offset, const uint8_t* bytes, const uint64_t num_bytes);

      static inline void
      UpdateUbo(const uint8_t* bytes, const uint64_t num_bytes) {
        return UpdateUbo(0, bytes, num_bytes);
      }

      static inline void
      DeleteUbo(const UboId id) {
        return DeleteUbos(&id, 1);
      }

      static inline void
      UnbindUbo() {
        return BindUbo(0);
      }
    public:
      ~Ubo() override;
      std::string ToString() const override;

      bool Accept(UboVisitor* vis) {
        PRT_ASSERT(vis);
        return vis->VisitUbo(this);
      }
      
      inline rx::observable<UboEvent*> OnEvent() const {
        return OnUboEvent(GetId());
      }

#define DEFINE_ON_UBO_EVENT(Name)                           \
      inline rx::observable<Name##Event*>                   \
      On##Name##Event() const {                             \
        return ubo::On##Name##Event(GetId());               \
      }
      FOR_EACH_UBO_EVENT(DEFINE_ON_UBO_EVENT)
#undef DEFINE_ON_UBO_EVENT
    };

    typedef std::set<Ubo*, Ubo::IdComparator> UboSet;

    const UboSet& GetAllUbos();
    uword GetTotalNumberOfUbos();
    bool VisitAllUbos(UboVisitor* vis);
  }
  using ubo::Ubo;
}

#endif //PRT_UBO_H