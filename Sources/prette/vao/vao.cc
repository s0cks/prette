#include "prette/vao/vao.h"

#include <set>
#include <sstream>

#include "prette/vao/vao_json.h"

namespace prt::vao {
  static VaoEventSubject events_;
  static VaoSet all_;

  static inline void
  Register(Vao* vao) {
    PRT_ASSERT(vao);
    const auto [iter,success] = all_.insert(vao);
    LOG_IF(ERROR, !success) << "failed to register: " << vao->ToString();
  }

  static inline void
  Deregister(Vao* vao) {
    PRT_ASSERT(vao);
    const auto removed = all_.erase(vao);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << vao->ToString();
  }

  rx::observable<VaoEvent*> OnEvent() {
    return events_.get_observable();
  }

  Vao::Vao(const VaoId id, const Metadata& meta):
    ObjectTemplate(id) {
    SetMeta(meta);
    Register(this);
  }

  Vao::Vao(const VaoId id, const VaoBuilder* builder):
    ObjectTemplate(id) {
    if(builder)
      SetMeta(builder->GetMeta());
    Register(this);
  }

  Vao::~Vao() {
    Deregister(this);
  }

  void Vao::BindVao(const VaoId id) {
    PRT_ASSERT(IsValidVaoId(id) || id == 0);
    glBindVertexArray(id);
    CHECK_GL(FATAL);
  }

  bool Vao::Accept(VaoVisitor* vis) {
    PRT_ASSERT(vis);
    return vis->VisitVao(this);
  }

  void Vao::DeleteVaos(const VaoId* ids, const uint64_t num_ids) {
    PRT_ASSERT(ids != nullptr);
    PRT_ASSERT(num_ids >= 1);
    glDeleteVertexArrays(num_ids, ids);
    CHECK_GL(FATAL);
  }

  void Vao::Publish(VaoEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  std::string Vao::ToString() const {
    std::stringstream ss;
    ss << "Vao(";
    ss << "meta=" << GetMeta() << ", ";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  Vao* Vao::FromJson(const uri::Uri& uri) {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }

  const VaoSet& GetAllVaos() {
    return all_;
  }

  bool VisitAllVaos(VaoVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& vao : all_) {
      if(!vao->Accept(vis))
        return false;
    }
    return true;
  }
  
  uword GetTotalNumberOfVaos() {
    return all_.size();
  }
}