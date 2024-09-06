#ifndef PRT_QUERY_H
#define PRT_QUERY_H

#include <set>
#include <ostream>
#include "prette/query/query_id.h"
#include "prette/query/query_target.h"
#include "prette/query/query_property.h"

namespace prt::query {
  class Query;
  class QueryVisitor {
  protected:
    QueryVisitor() = default;
  public:
    virtual ~QueryVisitor() = default;
    virtual bool VisitQuery(Query* q) = 0;
  };

  class Query {
    friend class QueryScope;
    friend class QueryTimeScope;
  public:
    struct Comparator {
      bool operator()(Query* lhs, Query* rhs) const {
        return lhs->GetId() < rhs->GetId();
      }
    };
  private:
    static void BeginQuery(const Target target, const QueryId id);
    static void EndQuery(const Target target);
    static void DeleteQueries(QueryId* ids, const int num_ids);
    static void CounterQuery(const QueryId id);

    static int32_t GetQueryInt(const QueryId id, const Property name);
    static uint32_t GetQueryUInt(const QueryId id, const Property name);
    static int64_t GetQueryLong(const QueryId id, const Property nane);
    static uint64_t GetQueryULong(const QueryId id, const Property name);

    static inline void
    DeleteQuery(QueryId id) {
      return DeleteQueries(&id, 1);
    }
  private:
    QueryId id_;
    Target target_;

    Query(const QueryId id, const Target target);
  public:
    ~Query();

    QueryId GetId() const {
      return id_;
    }

    Target GetTarget() const {
      return target_;
    }

    inline bool IsSamplesPassed() const {
      return GetTarget() == Target::kSamplesPassed;
    }

    inline bool IsAnySamplesPassed() const {
      return GetTarget() == Target::kAnySamplesPassed;
    }

    inline bool IsPrimitivesGenerated() const {
      return GetTarget() == Target::kPrimitivesGenerated;
    }

    inline bool IsTransformFeedbackPrimitivesWritten() const {
      return GetTarget() == Target::kTransformFeedbackPrimitivesWritten;
    }

    inline bool IsTimeElapsed() const {
      return GetTarget() == Target::kTimeElapsed;
    }

    uint64_t GetTimestamp() const {
      PRT_ASSERT(IsAvailable());
      return GetQueryULong(GetId(), Property::kResult);
    }

    void Counter() {
      PRT_ASSERT(IsTimeElapsed());
      Query::CounterQuery(GetId());
    }

    bool Accept(QueryVisitor* vis);
    bool IsAvailable() const;
    std::string ToString() const;
  private:
    static Query* New(const QueryId id, const Target target);
  public:
    static inline Query*
    NewTimeElapsed() {
      const auto id = QueryId::GenerateId();
      PRT_ASSERT(id);
      return New(id, Target::kTimeElapsed);
    }
  };

  typedef std::set<Query*, Query::Comparator> QuerySet;

  const QuerySet& GetAllQueries();
  bool VisitAllQueries(QueryVisitor* vis);
}

namespace prt {
  using query::Query;
}

#endif //PRT_QUERY_H