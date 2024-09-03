#include "prette/ibo/ibo.h"

#include <sstream>
#include "prette/ibo/ibo_builder.h"

namespace prt::ibo {
  static IboEventSubject events_;
  static IboSet all_;

  static inline void
  Register(Ibo* ibo) {
    PRT_ASSERT(ibo);
    const auto pos = all_.insert(ibo);
    LOG_IF(ERROR, !pos.second) << "failed to register: " << ibo->ToString();
  }

  static inline void
  Deregister(Ibo* ibo) {
    PRT_ASSERT(ibo);
    const auto removed = all_.erase(ibo);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << ibo->ToString();
  }

  Ibo::Ibo(const IboId id,
           Class* cls,
           const uword length,
           const gfx::Usage usage):
    BufferObject(id, usage),
    cls_(cls),
    length_(length) {
    Register(this);
  }

  Ibo::~Ibo() {
    Deregister(this);
  }

  bool Ibo::Equals(Ibo* rhs) const {
    return rhs
        && GetUsage() == rhs->GetUsage()
        && GetLength() == rhs->GetLength()
        && GetClass()->Equals(rhs->GetClass());
  }

  bool Ibo::Accept(IboVisitor* vis) {
    PRT_ASSERT(vis);
    return vis->VisitIbo(this);
  }

  rx::observable<IboEvent*> Ibo::OnEvent() const {
    return OnIboEvent()
      .filter([this](IboEvent* event) {
        return event
            && event->GetIboId() == GetId();
      });
  }

  void Ibo::Publish(IboEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

  void Ibo::BindIbo(const IboId id) {
    glBindBuffer(kGlTarget, id);
    CHECK_GL(FATAL);
  }

  void Ibo::InitBufferData(const Region& region, const gfx::Usage usage) {
    glBufferData(kGlTarget, region.GetSize(), (const GLvoid*) region, usage);
    CHECK_GL(FATAL);
  }

  void Ibo::UpdateBufferData(const uword offset, const Region& region) {
    glBufferSubData(kGlTarget, offset, region.GetSize(), (const GLvoid*) region);
    CHECK_GL(FATAL);
  }

  rx::observable<IboEvent*> OnIboEvent() {
    return events_.get_observable();
  }

  std::string Ibo::ToString() const {
    using namespace units;
    std::stringstream ss;
    ss << "Ibo(";
    ss << "id=" << GetId() << ", ";
    ss << "class=" << GetClass()->ToString() << ", ";
    ss << "length=" << GetLength() << ", ";
    ss << "total_size=" << data::byte_t(GetTotalSize()) << ", ";
    ss << "usage=" << GetUsage();
    ss << ")";
    return ss.str();
  }

  const IboSet& GetAllIbos() {
    return all_;
  }

  uword GetTotalNumberOfIbos() {
    return all_.size();
  }

  bool VisitAllIbos(IboVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& ibo : all_) {
      if(!ibo->Accept(vis))
        return false;
    }
    return true;
  }
}