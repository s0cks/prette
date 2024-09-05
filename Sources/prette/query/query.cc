#include "prette/query/query.h"
#include <sstream>

namespace prt::query {
  static QuerySet all_;

  static inline void
  Register(Query* query) {
    PRT_ASSERT(query);
    const auto [iter,success] = all_.insert(query);
    LOG_IF(ERROR, !success) << "failed to register: " << query->ToString();
  }

  static inline void
  Deregister(Query* query) {
    PRT_ASSERT(query);
    const auto removed = all_.erase(query);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << query->ToString();
  }

  void Query::BeginQuery(const Target target, const QueryId id) {
    PRT_ASSERT(IsValidQueryId(id));
    glBeginQuery(target, id);
    CHECK_GL;
  }

  void Query::EndQuery(const Target target) {
    glEndQuery(target);
    CHECK_GL;
  }

  void Query::DeleteQueries(QueryId* ids, const int num_ids) {
    PRT_ASSERT(num_ids >= 1);
    glDeleteQueries(num_ids, ids);
    CHECK_GL;
  }

  int32_t Query::GetQueryInt(const QueryId id, const Property name) {
    int32_t result = 0;
    glGetQueryObjectiv(id, name, &result);
    CHECK_GL;
    return result;
  }

  uint32_t Query::GetQueryUInt(const QueryId id, const Property name) {
    uint32_t result = 0;
    glGetQueryObjectuiv(id, name, &result);
    CHECK_GL;
    return result;
  }

  int64_t Query::GetQueryLong(const QueryId id, const Property name) {
    int64_t result = 0;
    glGetQueryObjecti64v(id, name, &result);
    CHECK_GL;
    return result; 
  }

  uint64_t Query::GetQueryULong(const QueryId id, const Property name) {
    uint64_t result = 0;
    glGetQueryObjectui64v(id, name, &result);
    CHECK_GL;
    return result;
  }

  Query::Query(const QueryId id, const Target target):
    id_(id),
    target_(target) {
    PRT_ASSERT(IsValidQueryId(id));
    Register(this);
  }

  Query::~Query() {
    Deregister(this);
  }

  bool Query::IsAvailable() const {
    GLint done = GL_FALSE;
    glGetQueryObjectiv(id_, GL_QUERY_RESULT_AVAILABLE, &done);
    CHECK_GL;
    return done == GL_TRUE;
  }

  bool Query::Accept(QueryVisitor* vis) {
    PRT_ASSERT(vis);
    return vis->VisitQuery(this);
  }

  std::string Query::ToString() const {
    std::stringstream ss;
    ss << "Query(";
    ss << "id=" << GetId() << ", ";
    ss << "target=" << GetTarget();
    ss << ")";
    return ss.str();
  }

  const QuerySet& GetAllQueries() {
    return all_;
  }

  bool VisitAllQueries(QueryVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& query : all_) {
      if(!query->Accept(vis))
        return false;
    }
    return true;
  }
}