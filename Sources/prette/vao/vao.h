#ifndef PRT_VAO_H
#define PRT_VAO_H

#include "prette/uri.h"
#include "prette/gfx_object.h"
#include "prette/vao/vao_id.h"
#include "prette/vao/vao_events.h"
#include "prette/vao/vao_builder.h"

namespace prt {
  namespace vao {
    class Vao;
    class VaoVisitor {
    protected:
      VaoVisitor() = default;
    public:
      virtual ~VaoVisitor() = default;
      virtual bool VisitVao(Vao* vao) = 0;
    };

    rx::observable<VaoEvent*> OnEvent();

    static inline rx::observable<VaoEvent*>
    OnEvent(const VaoId id) {
      return OnEvent()
        .filter(VaoEvent::FilterBy(id));
    }

#define DEFINE_ON_VAO_EVENT(Name)                                       \
    static inline rx::observable<Name##Event*>                          \
    On##Name##Event() {                                                 \
      return OnEvent()                                                  \
        .filter(Name##Event::Filter)                                    \
        .map(Name##Event::Cast);                                        \
    }
    FOR_EACH_VAO_EVENT(DEFINE_ON_VAO_EVENT)
#undef DEFINE_ON_VAO_EVENT

    class VaoBuilder;
    class Vao : public gfx::ObjectTemplate<VaoId>,
                public VaoEventSource {
      friend class VaoTest;
      friend class VaoFinalizer;
      friend class VaoBindScope;
      friend class VaoBuilder;
    public:
      struct VaoIdComparator {
        bool operator() (Vao* lhs, Vao* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    protected:
      static void BindVao(const VaoId id);
      static void DeleteVaos(const VaoId* ids, const uint64_t num_ids);
      static void Publish(VaoEvent* event);

      template<class E, typename... Args>
      static inline void
      Publish(Args... args) {
        E event(args...);
        return Publish((VaoEvent*) &event);
      }

      static inline void
      DeleteVao(const VaoId id) {
        return DeleteVaos(&id, 1);
      }
      
      static inline void
      UnbindVao() {
        return BindVao(kInvalidVaoId);
      }
    protected:
      Vao(const VaoId id, const Metadata& meta = {});
      Vao(const VaoId id, const VaoBuilder* builder);
    public:
      ~Vao() override;
      bool Accept(VaoVisitor* vis);
      std::string ToString() const override;

      inline bool IsValid() const {
        return IsValidVaoId(GetId());
      }

      inline bool IsInvalid() const {
        return IsInvalidVaoId(GetId());
      }

      rx::observable<VaoEvent*> OnEvent() const override {
        return vao::OnEvent(GetId());
      }
    public:
      static Vao* FromJson(const uri::Uri& uri);

      static inline Vao*
      New(const VaoId id, const Metadata& meta = {}) {
        VaoBuilder builder(id);
        builder.SetMeta(meta);
        return builder.Build();
      }

      static inline Vao*
      New(const Metadata& meta = {}) {
        return New(kInvalidVaoId, meta);
      }
    };

    typedef std::set<Vao*, Vao::VaoIdComparator> VaoSet;

    const VaoSet& GetAllVaos();
    bool VisitAllVaos(VaoVisitor* vis);
    uword GetTotalNumberOfVaos();
  }
  using vao::Vao;
}

#endif //PRT_VAO_H