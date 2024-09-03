#include "prette/fbo/fbo.h"

namespace prt::fbo {
  using namespace texture;

  static FboSet all_;
  static FboEventSubject events_;

  static inline void
  Register(Fbo* fbo) {
    PRT_ASSERT(fbo);
    const auto pos = all_.insert(fbo);
    LOG_IF(ERROR, !pos.second) << "failed to register: " << fbo->ToString();
  }

  static inline void
  Deregister(Fbo* fbo) {
    PRT_ASSERT(fbo);
    const auto removed = all_.erase(fbo);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << fbo->ToString();
  }

  Fbo::Fbo(const FboId id, const AttachmentSet& attachments):
    Object(),
    id_(id),
    attachments_(attachments) {
    Register(this);
  }

  Fbo::~Fbo() {
    Deregister(this);
  }

  FboEventObservable Fbo::OnEvent() const {
    return events_.get_observable();
  }

  rx::observable<FboEvent*> OnFboEvent() {
    return events_.get_observable();
  }

  rx::observable<FboEvent*> OnFboEvent(const FboId id) {
    return OnFboEvent()
      .filter([id](FboEvent* event) {
        return event
            && event->GetFboId() == id;
      });
  }

  std::string Fbo::ToString() const {
    std::stringstream ss;
    ss << "Fbo(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  Fbo* Fbo::New(const FboId id, const AttachmentSet& attachments) {
    const auto fbo = new Fbo(id, attachments);
    PRT_ASSERT(fbo);
    PublishEvent<FboCreatedEvent>(fbo);
    return fbo;
  }

  void Fbo::PublishEvent(FboEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

  void Fbo::BindFbo(const FboId id) {
    PRT_ASSERT(IsValidFboId(id));
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    CHECK_GL(FATAL);
  }

  void Fbo::DeleteFbos(const FboId* ids, const int num_ids) {
    PRT_ASSERT(num_ids >= 1);
    glDeleteFramebuffers(num_ids, ids);
    CHECK_GL(FATAL);
  }

  static inline bool
  IsValidTextureTarget(const texture::TextureTarget target) {
    switch(target) {
      case texture::k2D:
        return true; //TODO: need to support cube maps at some point
      default:
        return false;
    }
  }

  void Fbo::AttachTexture(const FboTarget target,
                          const AttachmentPoint p,
                          const TextureTarget textarget,
                          const TextureId texid,
                          const int level) {
    PRT_ASSERT(IsValidTextureTarget(textarget));
    PRT_ASSERT(level == 0); // TODO: ??
    PRT_ASSERT(IsValidTextureId(texid));
    glFramebufferTexture2D(target, p, textarget, texid, level);
    CHECK_GL(FATAL);
  }

  const FboSet& GetAllFbos() {
    return all_;
  }

  uword GetTotalNumberOfFbos() {
    return all_.size();
  }

  bool VisitAllFbos(FboVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& fbo : all_) {
      if(!fbo->Accept(vis))
        return false;
    }
    return true;
  }
}