#include "prette/ubo/ubo.h"
#include <sstream>

namespace prt::ubo {
  static rx::subject<UboEvent*> events_;
  static UboSet all_;

  static inline void
  Register(Ubo* ubo) {
    PRT_ASSERT(ubo);
    const auto pos = all_.insert(ubo);
    LOG_IF(ERROR, !pos.second) << "failed to register: " << ubo->ToString();
  }

  static inline void
  Deregister(Ubo* ubo) {
    PRT_ASSERT(ubo);
    const auto removed = all_.erase(ubo);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << ubo->ToString();
  }

  rx::observable<UboEvent*> OnUboEvent() {
    return events_.get_observable();
  }

  void Ubo::Publish(UboEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

  void Ubo::BindUbo(const UboId id) {
    PRT_ASSERT(IsValidUboId(id));
    glBindBuffer(Ubo::kGlTarget, id);
    CHECK_GL(FATAL);
  }

  void Ubo::DeleteUbos(const UboId* ids, const uint64_t num_ids) {
    PRT_ASSERT(num_ids > 0);
    glDeleteBuffers(num_ids, ids);
    CHECK_GL(FATAL);
  }

  void Ubo::InitUbo(const uint8_t* bytes, const uint64_t num_bytes, const GLenum usage) {
    PRT_ASSERT(num_bytes > 0);
    glBufferData(Ubo::kGlTarget, num_bytes, bytes, usage);
    CHECK_GL(FATAL);
  }

  void Ubo::UpdateUbo(const uint64_t offset, const uint8_t* bytes, const uint64_t num_bytes) {
    PRT_ASSERT(offset >= 0);
    PRT_ASSERT(num_bytes > 0);
    glBufferSubData(Ubo::kGlTarget, offset, num_bytes, bytes);
    CHECK_GL(FATAL);
  }

  Ubo::Ubo(const UboId id,
           const uword elem_size,
           const uword length,
           const gfx::Usage usage):
    BufferObject<UboId>(id, usage),
    length_(length),
    elem_size_(elem_size) {
    Register(this);
    Publish<UboCreatedEvent>(this);
  }

  Ubo::~Ubo() {
    Deregister(this);
    Publish<UboDestroyedEvent>(this);
  }

  std::string Ubo::ToString() const {
    std::stringstream ss;
    ss << "Ubo(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  const UboSet& GetAllUbos() {
    return all_;
  }

  uword GetTotalNumberOfUbos() {
    return all_.size();
  }

  bool VisitAllUbos(UboVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& ubo : all_) {
      if(!ubo->Accept(vis))
        return false;
    }
    return true;
  }
}