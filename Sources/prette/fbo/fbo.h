#ifndef PRT_FRAMEBUFFER_H
#define PRT_FRAMEBUFFER_H

#include "prette/fbo/fbo_id.h"
#include "prette/fbo/fbo_events.h"
#include "prette/fbo/fbo_attachment.h"
#include "prette/fbo/fbo_attachments.h"

namespace prt {
  namespace fbo {
    class Fbo;
    class FboVisitor {
    protected:
      FboVisitor() = default;
    public:
      virtual ~FboVisitor() = default;
      virtual bool VisitFbo(Fbo* fbo) = 0;
    };

    rx::observable<FboEvent*> OnFboEvent();
    rx::observable<FboEvent*> OnFboEvent(const FboId id);

#define DEFINE_ON_FBO_EVENT(Name)                                   \
    static inline rx::observable<Name##Event*>                      \
    On##Name##Event() {                                             \
      return OnFboEvent()                                           \
        .filter(Name##Event::Filter)                                \
        .map(Name##Event::Cast);                                    \
    }                                                               \
    static inline rx::observable<Name##Event*>                      \
    On##Name##Event(const FboId id) {                               \
      return OnFboEvent()                                           \
        .filter(Name##Event::FilterBy(id))                          \
        .map(Name##Event::Cast);                                    \
    }
    FOR_EACH_FBO_EVENT(DEFINE_ON_FBO_EVENT)
#undef DEFINE_ON_FBO_EVENT

    class FboFactory;
    class Fbo : public Object,
                public FboEventSource {
      friend class FboFactory;
      friend class FboBindScope;
      friend class ColorAttachment;
    public:
      struct IdComparator {
        bool operator()(Fbo* lhs, Fbo* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    private:
      static void PublishEvent(FboEvent* event);

      template<typename E, typename... Args>
      static inline void
      PublishEvent(Args... args) {
        E event(args...);
        return PublishEvent((FboEvent*) &event);
      }

      static void BindFbo(const FboId id);
      static void DeleteFbos(const FboId* ids, const int num_ids);

      static void AttachTexture(const FboTarget target,
                                const AttachmentPoint p,
                                const texture::TextureTarget textarget,
                                const texture::TextureId texid,
                                const int level = 0);

      static inline void
      AttachTexture2D(const FboTarget target,
                      const AttachmentPoint p,
                      const texture::TextureId texid,
                      const int level = 0) {
        return AttachTexture(target, p, texture::k2D, texid, level);
      }

      static inline void
      BindDefaultFbo() {
        return BindFbo(kDefaultFboId);
      }

      static inline void
      DeleteFbo(const FboId id) {
        return DeleteFbos(&id, 1);
      }
    protected:
      FboId id_;
      AttachmentSet attachments_;
      
      void Publish(FboEvent* event) override {
        return PublishEvent(event);
      }
    private:
      Fbo(const FboId id, const AttachmentSet& attachments);
    public:
      ~Fbo() override;

      FboId GetId() const {
        return id_;
      }

      const AttachmentSet& GetAttachments() const {
        return attachments_;
      }

      bool Accept(FboVisitor* vis) {
        PRT_ASSERT(vis);
        return vis->VisitFbo(this);
      }

      std::string ToString() const override;
      FboEventObservable OnEvent() const override;
    private:
      static Fbo* New(const FboId id, const AttachmentSet& attachments);
    public:
      static Fbo* New();
    };

    typedef std::set<Fbo*, Fbo::IdComparator> FboSet;

    const FboSet& GetAllFbos();
    uword GetTotalNumberOfFbos();
    bool VisitAllFbos(FboVisitor* vis);
  }
  using fbo::Fbo;
}

#endif //PRT_FRAMEBUFFER_H