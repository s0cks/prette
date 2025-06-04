#include "prette/component/component_table.h"

#include <gflags/gflags.h>

namespace prt {
DEFINE_uint32(component_table_size, kDefaultComponentTableSize, "");

ComponentTable::ComponentTable() {
  // do nothing
}

ComponentTable::~ComponentTable() {
  // do nothing
}
}  // namespace prt