#include "prette/vbo/vbo.h"

#include <units.h>

namespace prt::vbo {
  static VboSet all_;
  static rx::subject<VboEvent*> events_;

  static inline void
  Register(Vbo* vbo) {
    PRT_ASSERT(vbo);
    const auto pos = all_.insert(vbo);
    LOG_IF(ERROR, !pos.second) << "failed to register: " << vbo->ToString();
  }

  static inline void
  Deregister(Vbo* vbo) {
    PRT_ASSERT(vbo);
    const auto removed = all_.erase(vbo);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << vbo->ToString();
  }

  Vbo::Vbo(const VboId id,
           Class* cls,
           const uword length,
           const gfx::Usage usage):
    gfx::BufferObject<VboId, kGlTarget>(id, cls, length, usage) {
    Register(this);
  }

  Vbo::~Vbo() {
    Deregister(this);
  }

  rx::observable<VboEvent*> OnVboEvent() {
    return events_.get_observable();
  }

  rx::observable<VboEvent*> OnVboEvent(const VboId id) {
    return OnVboEvent()
      .filter([id](VboEvent* event) {
        return event
            && event->GetVboId() == id;
      });
  }

  void Vbo::PublishEvent(VboEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  bool Vbo::Accept(VboVisitor* vis) {
    PRT_ASSERT(vis);
    return vis->VisitVbo(this);
  }

  std::string Vbo::ToString() const {
    using namespace units;
    std::stringstream ss;
    ss << "Vbo(";
    ss << "id=" << GetId() << ", ";
    ss << "length=" << GetLength() << ", ";
    ss << "vertex_size=" << data::byte_t(GetElementSize()) << ", ";
    ss << "total_size=" << data::byte_t(GetTotalSize()) << ", ";
    ss << "usage=" << GetUsage();
    ss << ")";
    return ss.str();
  }

  Vbo* Vbo::New(const VboId id,
                Class* cls,
                const uword length,
                const gfx::Usage usage) {
    PRT_ASSERT(id.IsValid());
    PRT_ASSERT(cls);
    PRT_ASSERT(length >= 0);
    const auto vbo = new Vbo(id, cls, length, usage);

    //TODO: remove
    VboCreatedEvent event(vbo);
    PublishEvent(&event);

    return vbo;
  }

  const VboSet& GetAllVbos() {
    return all_;
  }

  uword GetTotalNumberOfVbos() {
    return all_.size();
  }

  bool VisitAllVbos(VboVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& vbo : all_) {
      if(!vbo->Accept(vis))
        return false;
    }
    return true;
  }

  bool VisitAllVbos(VboVisitor* vis, const VboPredicate& predicate) {
    PRT_ASSERT(vis);
    for(const auto& vbo : all_) {
      if(predicate(vbo) && !vbo->Accept(vis))
        return false;
    }
    return true;
  }
}