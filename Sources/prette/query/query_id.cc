#include "prette/query/query_id.h"

namespace prt::query {
  void GenerateQueryIds(QueryId* ids, const int num_ids) {
    PRT_ASSERT(num_ids >= 1);
    glGenQueries(num_ids, ids);
    CHECK_GL(FATAL);
  }

  bool IsQuery(const QueryId id) {
    return glIsQuery(id) == GL_TRUE;
  }
}