#ifndef PRT_VBO_H
#define PRT_VBO_H

#include "prette/class.h"
#include "prette/region.h"
#include "prette/vbo/vbo_id.h"
#include "prette/vbo/vbo_scope.h"
#include "prette/vbo/vbo_target.h"
#include "prette/vbo/vbo_events.h"
#include "prette/vbo/vbo_visitor.h"
#include "prette/vbo/vbo_builder.h"

#include "prette/gfx_usage.h"
#include "prette/gfx_buffer_object.h"

namespace prt {
  namespace vbo {
    VboEventObservable OnVboEvent();
    VboEventObservable OnVboEvent(const VboId id);

#define DEFINE_ON_VBO_EVENT(Name)                 \
    static inline Name##EventObservable           \
    On##Name##Event() {                           \
      return OnVboEvent()                         \
        .filter(Name##Event::Filter)              \
        .map(Name##Event::Cast);                  \
    }                                             \
    static inline Name##EventObservable           \
    On##Name##Event(const VboId id) {             \
      return OnVboEvent()                         \
        .filter(Name##Event::FilterBy(id))        \
        .map(Name##Event::Cast);                  \
    }
    FOR_EACH_VBO_EVENT(DEFINE_ON_VBO_EVENT)
#undef DEFINE_ON_VBO_EVENT

    class Vbo : public gfx::BufferObject<VboId, kGlTarget>,
                public VboEventSource {
      friend class VboScope;
      friend class VboBindScope;

      template<typename V>
      friend class VboUpdateScope;
      friend class VboUpdateScopeBase;
      friend class VboBuilderBase;
    public:
      struct IdComparator {
        bool operator()(Vbo* lhs, Vbo* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    private:
      static void PublishEvent(VboEvent* event);

      static inline void
      BindDefaultVbo() {
        return Bind(kDefaultVboId);
      }

      static inline void
      Unbind() {
        return Bind(kDefaultVboId);
      }
    protected:
      Vbo(const VboId id,
          Class* cls,
          const uword length,
          const gfx::Usage usage);
      
      void Publish(VboEvent* event) override {
        return PublishEvent(event);
      }
    public:
      ~Vbo() override;

      bool Accept(VboVisitor* vis);
      std::string ToString() const override;

      VboEventObservable OnEvent() const override {
        return vbo::OnVboEvent(GetId());
      }

#define DEFINE_ON_EVENT(Name)                             \
      Name##EventObservable On##Name() const override {   \
        return vbo::On##Name##Event(GetId());             \
      }
      FOR_EACH_VBO_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
    private:
      static Vbo* New(const VboId id, Class* cls, const uword length, const gfx::Usage usage);
    };

    static inline std::ostream&
    operator<<(std::ostream& stream, const Vbo*& rhs) {
      return stream << rhs->ToString();
    }
    
    typedef std::set<Vbo*, Vbo::IdComparator> VboSet;

    typedef std::function<bool(Vbo*)> VboPredicate;

    const VboSet& GetAllVbos();
    uword GetTotalNumberOfVbos();
    bool VisitAllVbos(VboVisitor* vis);
    bool VisitAllVbos(VboVisitor* vis, const VboPredicate& predicate);

    static inline bool
    VisitAllVbosWithClass(VboVisitor* vis, Class* cls) {
      PRT_ASSERT(vis);
      PRT_ASSERT(cls);
      return VisitAllVbos(vis, [cls](Vbo* vbo) {
        return vbo
            && vbo->GetClass()->Equals(cls);
      });
    }

#ifdef PRT_DEBUG
    class VboPrinter : public VboVisitor {
      using Severity=google::LogSeverity;
    private:
      Severity severity_;
    public:
      explicit VboPrinter(const Severity severity = google::INFO):
        VboVisitor(),
        severity_(severity) {
      }
      ~VboPrinter() override = default;

      Severity GetSeverity() const {
        return severity_;
      }
      
      bool VisitVbo(Vbo* vbo) override {
        PRT_ASSERT(vbo);
        LOG_AT_LEVEL(GetSeverity()) << vbo->ToString();
        return true;
      }
    };

    static inline bool
    PrintAllVbos(const google::LogSeverity severity = google::INFO) {
      DLOG(INFO) << GetTotalNumberOfVbos() << " Vertex Buffer Objects: ";
      VboPrinter printer(severity);
      return VisitAllVbos(&printer);
    }
#endif //PRT_DEBUG
  }
  using vbo::Vbo;
}

#endif //PRT_VBO_H