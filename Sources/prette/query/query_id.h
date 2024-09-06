#ifndef PRT_QUERY_ID_H
#define PRT_QUERY_ID_H

#include "prette/gfx.h"
#include "prette/gfx_object_id.h"

namespace prt::query {
  typedef gfx::ObjectId<GLuint, &glGenQueries> QueryId;

  static constexpr const QueryId kDefaultQueryId = 0;

  bool IsQuery(const QueryId id);
}

#endif //PRT_QUERY_ID_H