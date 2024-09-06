#include "prette/query/query_scope.h"
#include "prette/query/query.h"

namespace prt::query {
  QueryScope::QueryScope(Query* q):
    query_(q) {
    PRT_ASSERT(q);
    Query::BeginQuery(GetQueryTarget(), GetQueryId());
  }

  QueryScope::~QueryScope() {
    Query::EndQuery(GetQueryTarget());
  }

  QueryId QueryScope::GetQueryId() const {
    return GetQuery()->GetId();
  }

  Target QueryScope::GetQueryTarget() const {
    return GetQuery()->GetTarget();
  }

  QueryTimeScope::QueryTimeScope(Query* query):
    QueryScope(query) {
    PRT_ASSERT(query->IsTimeElapsed());
  }

  uword QueryTimeScope::GetTimestamp() const {
    Query::CounterQuery(GetQueryId());
    return Query::GetQueryULong(GetQueryId(), kResult);
  }
}