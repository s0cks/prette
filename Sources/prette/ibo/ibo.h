#ifndef PRT_INDEX_BUFFER_H
#define PRT_INDEX_BUFFER_H

#include <vector>
#include "prette/class.h"
#include "prette/region.h"
#include "prette/gfx_usage.h"
#include "prette/gfx_target.h"
#include "prette/gfx_buffer_object.h"

#include "prette/ibo/ibo_id.h"
#include "prette/ibo/ibo_events.h"

namespace prt {
  namespace ibo {
    class Ibo;
    class IboVisitor {
    protected:
      IboVisitor() = default;
    public:
      virtual ~IboVisitor() = default;
      virtual bool VisitIbo(Ibo* ibo) = 0;
    };

    rx::observable<IboEvent*> OnIboEvent();

#define DEFINE_ON_IBO_EVENT(Name)                                 \
    static inline rx::observable<Name##Event*>                    \
    On##Name##Event() {                                           \
      return OnIboEvent()                                         \
        .filter(Name##Event::Filter)                              \
        .map(Name##Event::Cast);                                  \
    }                                                             \
    static inline rx::observable<Name##Event*>                    \
    On##Name##Event(const IboId id) {                             \
      return OnIboEvent()                                         \
        .filter(Name##Event::FilterBy(id))                        \
        .map(Name##Event::Cast);                                  \
    }
    FOR_EACH_IBO_EVENT(DEFINE_ON_IBO_EVENT)
#undef DEFINE_ON_IBO_EVENT

    static constexpr const auto kGlTarget = gfx::kIboTarget;

    class Ibo : public gfx::BufferObject<IboId, kGlTarget>,
                public IboEventSource {
      friend class IboBuilderBase;
      
      template<typename I>
      friend class IboReadScope;
      friend class IboBindScope;

      template<typename I>
      friend class IboUpdateScope;
    public:
      static constexpr const auto kDefaultUsage = gfx::kDynamicDrawUsage;
      struct IdComparator {
        bool operator() (Ibo* lhs, Ibo* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    private:
      // bind
      static void BindIbo(const IboId id);
      
      static inline void
      BindDefaultIbo() { //TODO: refactor
        return BindIbo(kInvalidIboId);
      }

      // init
      static void InitBufferData(const Region& region, const gfx::Usage usage);

      static inline void
      InitBufferData(const uword num_bytes, const gfx::Usage usage) {
        return InitBufferData(Region(0, num_bytes), usage);
      }

      // update
      static void UpdateBufferData(const uword offset,  const Region& region);
      
      static inline void
      UpdateBufferData(const Region& region) {
        return UpdateBufferData(0, region);
      }

      // publish events
      static void Publish(IboEvent* event);

      template<typename E, typename... Args>
      static inline void
      Publish(Args... args) {
        E event(args...);
        return Publish((IboEvent*) &event);
      }
    protected:
      Ibo(const IboId id,
          Class* cls,
          const uword length,
          const gfx::Usage usage);
    public:
      ~Ibo() override;

      GLenum GetGlType() const {
        return GetClass()->GetGlType();
      }

      std::string ToString() const override;
      bool Equals(Ibo* ibo) const;
      bool Accept(IboVisitor* vis);
      rx::observable<IboEvent*> OnEvent() const override;
    };

    typedef std::set<Ibo*, Ibo::IdComparator> IboSet;

    const IboSet& GetAllIbos();
    uword GetTotalNumberOfIbos();
    bool VisitAllIbos(IboVisitor* vis);
  }
  using ibo::Ibo;
}

#endif //PRT_INDEX_BUFFER_H