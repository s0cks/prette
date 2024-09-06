#ifndef PRT_QUERY_SCOPE_H
#define PRT_QUERY_SCOPE_H

#include "prette/query/query_id.h"
#include "prette/query/query_target.h"

namespace prt::query {
  class Query;
  class QueryScope {
  private:
    Query* query_;
  protected:
    explicit QueryScope(Query* query);
  public:
    virtual ~QueryScope();

    Query* GetQuery() const {
      return query_;
    }

    QueryId GetQueryId() const;
    Target GetQueryTarget() const;
  };

  class QueryTimeScope : public QueryScope {
  public:
    explicit QueryTimeScope(Query* query);
    ~QueryTimeScope() override = default;
    uword GetTimestamp() const;
  };
}

#endif //PRT_QUERY_SCOPE_H