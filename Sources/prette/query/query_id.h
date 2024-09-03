#ifndef PRT_QUERY_ID_H
#define PRT_QUERY_ID_H

#include "prette/gfx.h"

namespace prt::query {
  typedef GLuint QueryId;

  void GenerateQueryIds(QueryId* ids, const int num_ids);

  static inline QueryId
  GenerateQueryId() {
    QueryId id;
    GenerateQueryIds(&id, 1);
    return id;
  }

  static constexpr const QueryId kInvalidQueryId = 0;

  static inline bool
  IsValidQueryId(const QueryId id) {
    return id != kInvalidQueryId;
  }

  static inline bool
  IsInvalidQueryId(const QueryId id) {
    return id == kInvalidQueryId;
  }

  bool IsQuery(const QueryId id);
}

#endif //PRT_QUERY_ID_H