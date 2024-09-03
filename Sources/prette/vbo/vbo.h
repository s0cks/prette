#ifndef PRT_VBO_H
#define PRT_VBO_H

#include "prette/class.h"
#include "prette/region.h"
#include "prette/vbo/vbo_id.h"
#include "prette/vbo/vbo_events.h"

#include "prette/gfx_usage.h"
#include "prette/gfx_buffer_object.h"

namespace prt {
  namespace vbo {
    class Vbo;
    class VboVisitor {
    protected:
      VboVisitor() = default;
    public:
      virtual ~VboVisitor() = default;
      virtual bool VisitVbo(Vbo* vbo) = 0;
    };

    rx::observable<VboEvent*> OnVboEvent();
    rx::observable<VboEvent*> OnVboEvent(const VboId id);

#define DEFINE_ON_VBO_EVENT(Name)                 \
    static inline rx::observable<Name##Event*>    \
    On##Name##Event() {                           \
      return OnVboEvent()                         \
        .filter(Name##Event::Filter)              \
        .map(Name##Event::Cast);                  \
    }                                             \
    static inline rx::observable<Name##Event*>    \
    On##Name##Event(const VboId id) {             \
      return OnVboEvent()                         \
        .filter(Name##Event::FilterBy(id))        \
        .map(Name##Event::Cast);                  \
    }
    FOR_EACH_VBO_EVENT(DEFINE_ON_VBO_EVENT)
#undef DEFINE_ON_VBO_EVENT

    class Vbo : public gfx::BufferObject<VboId>,
                public VboEventSource {
      friend class VboScope;
      friend class VboBindScope;

      template<typename V>
      friend class VboUpdateScope;
      friend class VboBuilderBase;
    public:
      static constexpr const auto kGlTarget = GL_ARRAY_BUFFER;
      struct IdComparator {
        bool operator()(Vbo* lhs, Vbo* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    private:
      static void PublishEvent(VboEvent* event);

      // bind
      static void BindVbo(const VboId id);

      static inline void
      BindDefaultVbo() {
        return BindVbo(kDefaultVboId);
      }

      static inline void
      Unbind() {
        return BindVbo(kDefaultVboId);
      }

      // init
      static void InitData(const Region& region, const gfx::Usage usage);

      static inline void
      InitData(const uword num_bytes, const gfx::Usage usage) {
        return InitData(Region(0, num_bytes), usage);
      }

      // write
      static void UpdateVboData(const uword offset, const Region& region);

      static inline void
      UpdateVboData(const Region& region) {
        return UpdateVboData(0, region); //TODO: causes issues if new size is larger than allocated size
      }
      
      // delete
      static void DeleteVbos(const VboId* ids, const int num_ids);

      static inline void
      DeleteVbo(const VboId id) {
        return DeleteVbos(&id, 1);
      }
    protected:
      Class* class_;
      uword length_;

      Vbo(const VboId id,
          Class* cls,
          const uword length,
          const gfx::Usage usage);
      
      void Publish(VboEvent* event) override {
        return PublishEvent(event);
      }

      void SetLength(const uword length) {
        length_ = length;
      }
    public:
      ~Vbo() override;

      Class* GetClass() const {
        return class_;
      }

      uword GetLength() const override {
        return length_;
      }

      uword GetElementSize() const override {
        return GetClass()->GetAllocationSize();
      }
      
      bool IsInvalid() const {
        return IsInvalidVboId(GetId());
      }

      bool IsValid() const {
        return IsValidVboId(GetId());
      }

      bool Accept(VboVisitor* vis);
      std::string ToString() const override;

      rx::observable<VboEvent*> OnEvent() const override {
        return vbo::OnVboEvent(GetId());
      }
    private:
      static Vbo* New(const VboId id, Class* cls, const uword length, const gfx::Usage usage);
    };

    typedef std::set<Vbo*, Vbo::IdComparator> VboSet;
    
    const VboSet& GetAllVbos();
    uword GetTotalNumberOfVbos();
    bool VisitAllVbos(VboVisitor* vis);

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